# PINTS for FITMAN version 0.4

# ---- System Libraries ---- #
import sys
import os
import datetime
import time
import glob
import platform

from PyQt5 import QtCore, QtGui, QtWidgets, uic
import subprocess as subproc

import multiprocessing as mp

from collections import defaultdict

from itertools import groupby

# ---- Math Libraries ---- #
import scipy as sp
import scipy.signal as spsg

import numpy as np
import math
from pyfftw.interfaces import scipy_fftpack as fftw

# ---- Plotting Libraries ---- #
import matplotlib as mpl;
mpl.use("Qt5Agg")
from matplotlib.backends.backend_qt5agg import (
	FigureCanvasQTAgg as FigureCanvas,
	NavigationToolbar2QT as NavigationToolbar)
import matplotlib.pyplot as plt; plt.style.use('ggplot')

# ---- Data Classes ---- #
from dataclasses import *

qtCreatorFile = "spices/ui/SPICeS.ui"
Ui_MainWindow, QtBaseClass = uic.loadUiType(qtCreatorFile)

# ---- Main Application Class ---- #
class MyApp(QtWidgets.QWidget, Ui_MainWindow):

	# ---- Methods to Set Up UI ---- #
	def __init__(self):
		QtWidgets.QWidget.__init__(self)
		Ui_MainWindow.__init__(self)
		self.setupUi(self)

		# Bind Buttons
		self.setDirectoryButton.clicked.connect(self.setWorkingDirectory)
		self.setDirectoryButton.setEnabled(True)

		self.outFileBrowseButton.clicked.connect(self.chooseOutputFile)
		self.outFileBrowseButton.setEnabled(False)

		self.outFileConfirmButton.clicked.connect(self.loadOutputFile)
		self.outFileConfirmButton.setEnabled(False)

		self.datFileBrowseButton.clicked.connect(self.chooseDatFile)
		self.datFileBrowseButton.setEnabled(False)

		self.datFileConfirmButton.clicked.connect(self.loadDatFile)
		self.datFileConfirmButton.setEnabled(False)

		# self.loadGroupsButton.clicked.connect(self.loadVisGroupFile)
		self.loadGroupsButton.setEnabled(False)

		# self.saveGroupsButton.clicked.connect(self.saveVisGroupFile)
		self.saveGroupsButton.setEnabled(False)

		self.plotButton.clicked.connect(self.plot)
		self.plotButton.setEnabled(False)

		# Setup for plotting in the "Plot Metabolites" and "Generate Guess File" Tabs
		self.canvas = [None]*1
		self.toolbar = [None]*1
		self.setPlot('Plot')

	def setPlot(self, tab):
		if tab == 'Plot':
			fig = plt.figure(1)
			self.addmpl(0, fig, self.plotFreq_mplvl)

	# ---- Methods to Load Files ---- #
	def setWorkingDirectory(self):
		self.workingDirectory = str(QtWidgets.QFileDialog.getExistingDirectory(self, 'Set Working Directory', os.path.expanduser('~')))
		if self.workingDirectory == '':
			self.workingDirectory = os.path.expanduser('~')

		self.outFileBrowseButton.setEnabled(True)
		self.datFileBrowseButton.setEnabled(True)

	def chooseOutputFile(self):
		self.outFileConfirmButton.setEnabled(False)
		prev = str(self.outFileInput.text())
		self.outFileInput.setText(str(QtWidgets.QFileDialog.getOpenFileName(self, 'Open Output File', self.workingDirectory, 'Output files (*.out)')[0]))
		self.outFileConfirmButton.setEnabled(True)
		if str(self.outFileInput.text()) == '':
			self.outFileInput.setText(str(prev))
			if str(prev) == '':
				self.outFileConfirmButton.setEnabled(False)

	def chooseDatFile(self):
		self.datFileConfirmButton.setEnabled(False)
		prev = str(self.datFileInput.text())
		self.datFileInput.setText(str(QtWidgets.QFileDialog.getOpenFileName(self, 'Open Dat File', self.workingDirectory, 'Dat files (*.dat)')[0]))
		self.datFileConfirmButton.setEnabled(True)
		if str(self.datFileInput.text()) == '':
			self.datFileInput.setText(str(prev))
			if str(prev) == '':
				self.datFileConfirmButton.setEnabled(False)

	def loadOutputFile(self):
		try:
			# load output file
			fit_file     = str(self.outFileInput.text())
			self.fit_out = OutputFile(fit_file)
			self.outFileInfoLabel.setText(fit_file.split('/')[-1] + "\nsuccessfully loaded.")

			# populate available metabolite list
			self.metabTextBrowser.clear()
			for metabolite in self.fit_out.metabolites_list:
				self.metabTextBrowser.append(str(metabolite) + ',')

			# reset buttons
			self.outFileBrowseButton.setEnabled(True)
			self.outFileConfirmButton.setEnabled(False)

		except Exception as e:
			self.outFileInfoLabel.setText("ERROR: " + str(e) + "\n>> Please try again.")

	def loadDatFile(self):
		try:
			# load dat file
			invivo_file     = str(self.datFileInput.text())
			self.invivo_dat = DatFile(invivo_file)
			self.datFileInfoLabel.setText(invivo_file.split('/')[-1] + "\nsuccessfully loaded.")

			# populate plotting parameters
			self.b0Input.setText(str(self.invivo_dat.b0))
			self.dwellTimeInput.setText(str(1/self.invivo_dat.fs))
			self.nInput.setText(str(self.invivo_dat.n))

			# reset buttons
			self.datFileBrowseButton.setEnabled(True)
			self.datFileConfirmButton.setEnabled(False)

			self.loadGroupsButton.setEnabled(True)
			self.saveGroupsButton.setEnabled(True)
			self.plotButton.setEnabled(True)

		except Exception as e:
			self.datFileInfoLabel.setText("ERROR: " + str(e) + "\n>> Please try again.")

	# ---- Methods to Calculate and Plot Spectra ---- #
	def addmpl(self, canvas_index, fig, vertical_layout):
		self.canvas[canvas_index] = FigureCanvas(fig)
		vertical_layout.addWidget(self.canvas[canvas_index])
		self.canvas[canvas_index].draw()

		self.toolbar[canvas_index] = NavigationToolbar(self.canvas[canvas_index], self, coordinates=True)
		vertical_layout.addWidget(self.toolbar[canvas_index])

	def plot(self):
		try:
			self.plotErrorLabel.setText('')

			plot_groups = []

			# Read Groups
			defined_groups = str(self.groupsTextEdit.toPlainText()).replace('\n','').replace(' ','').split(';')
			defined_groups = defined_groups[0:-1]

			# Parse each group as if defining a constraint group (recylcing code from PINTS)
			for group in defined_groups:
				group_info = group.split(',')
				group_name = group_info[0]
				group_members = group_info[1:]

				new_group = CSTGroup('', group_name, group_members, 0, 0)

				plot_groups.append(new_group)


			# Gather plotting variables
			sfactor = float(self.sfactorInput.text())
			pfactor = sp.deg2rad(float(self.pfactorInput.text()))
			VSHIFT  = float(self.VSHIFTInput.text())
			HSHIFT  = float(self.HSHIFTInput.text())

			b0 = float(self.b0Input.text())
			dwell_time = float(self.dwellTimeInput.text())
			acq_time = int(self.nInput.text())*dwell_time
			fs = 1/dwell_time
			t = sp.arange(0, acq_time, dwell_time)

			tableau20 = [(31, 119, 180), (174, 199, 232), (255, 127, 14), (255, 187, 120),    
						 (44, 160, 44), (152, 223, 138), (214, 39, 40), (255, 152, 150),    
						 (148, 103, 189), (195, 176, 213), (140, 86, 75), (196, 156, 148),    
						 (227, 119, 194), (247, 182, 210), (127, 127, 127), (199, 199, 199),    
						 (188, 189, 34), (219, 219, 141), (23, 190, 207), (158, 218, 229)]

			for i in range(len(tableau20)):    
				r, g, b = tableau20[i]    
				tableau20[i] = (r / 255., g / 255., b / 255.)

			# Calculate Summed Spectra
			fit_spec_sum = []
			for metabolite in self.fit_out.metabolites_list:
				fit_f, spec = self.fit_out.metabolites[metabolite].getSpec(0, b0, t, 0, 1, pfactor, 0, 0, fs)
				fit_spec_sum.append(spec)
			fit_spec_sum = np.real(np.sum(np.array(fit_spec_sum), axis=0))
			fit_spec_sum = fit_spec_sum


			# Calculate In-Vivo Spectra
			self.invivo_dat.signal = self.invivo_dat.signal * sp.exp(1j*pfactor)
			invivo_f, invivo_spec = self.invivo_dat.getSpec()
			invivo_spec = np.real(invivo_spec)

			# Calculate Fitted Spectra
			fit_spec = []; fit_spec_names = []
			for group in plot_groups:
				group_spec = []
				for member in group.members:
					fit_f, spec = self.fit_out.metabolites[member].getSpec(0, b0, t, 0, 1, pfactor, 0, 0, fs)
					group_spec.append(spec)
				group_spec = np.real(np.sum(np.array(group_spec), axis=0))
				group_spec = group_spec
				fit_spec.append(group_spec)
				fit_spec_names.append(group.name)

			plt.figure(1)
			plt.clf()
			
			ax = plt.subplot(111)
			ax.spines["top"].set_visible(False)    
			ax.spines["bottom"].set_visible(True)    
			ax.spines["right"].set_visible(False)    
			ax.spines["left"].set_visible(False)
			ax.get_xaxis().tick_bottom()
			plt.xlim(5,0)
			plt.tick_params(axis="both", which="both", bottom="on", top="off",    
							labelbottom="on", left="off", right="off", labelleft="off")

			for i, spec in enumerate(fit_spec):
				plt.plot(np.array(-fit_f)+sfactor, np.real(spec)+i*VSHIFT*np.amax(np.real(fit_spec_sum)), color=tableau20[(i+1)%np.size(tableau20)], lw=1.5)
				ax.text(5, i*VSHIFT*np.amax(np.real(fit_spec_sum)), fit_spec_names[i])

			if i == 0: i = 1
			plt.plot(np.array(-invivo_f)+sfactor+HSHIFT, np.real(invivo_spec)+(i+2)*VSHIFT*np.amax(np.real(invivo_spec)), color=tableau20[0], lw=1.5)
			plt.plot(np.array(-fit_f)+sfactor, np.real(fit_spec_sum)+(i+2)*VSHIFT*np.amax(np.real(invivo_spec)), color=tableau20[2], lw=1.5, alpha=0.8)

			plt.xlabel('ppm')

			self.canvas[0].draw()

			# Reset In-Vivo Spectra
			self.invivo_dat.signal = self.invivo_dat.signal * sp.exp(-1j*pfactor)

		except Exception as e:
			self.plotErrorLabel.setText('>> ERROR: ' + str(e) + '\n>> Could not plot. Please try again.')

# ---- Launch Application ---- #
if __name__ == "__main__":
	app = QtWidgets.QApplication(sys.argv)
	window = MyApp()
	window.show()
	sys.exit(app.exec_())