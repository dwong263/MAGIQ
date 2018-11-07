# SPICeS for FITMAN version 0.4

# ---- System Libraries ---- #
import sys
import os
import datetime
import time
import glob
import platform
import copy

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
import matplotlib.pyplot as plt

# ---- Data Classes ---- #
from dataclasses import *

qtCreatorFile = "apps/ui/apps.ui"
Ui_MainWindow, QtBaseClass = uic.loadUiType(qtCreatorFile)

# ---- Main Application Class ---- #
class MyApp(QtWidgets.QWidget, Ui_MainWindow):

	# ---- Methods to Set Up UI ---- #
	def __init__(self):
		QtWidgets.QWidget.__init__(self)
		Ui_MainWindow.__init__(self)
		self.setupUi(self)

		self.workingDirectory = os.path.expanduser('~')
		
		# Setup for plotting
		self.canvas = [None]*2
		self.toolbar = [None]*2

		# Bind buttons to methods in each tab
		self.setBindings('Water Removal')
		self.setBindings('Macromolecule Removal')

	def setBindings(self, tab):
		if tab == 'Water Removal':
			self.setPlot(tab)
		elif tab == 'Macromolecule Removal':
			self.filenameBrowseButton_fulldat_mmr.clicked.connect(self.chooseFullDatFile_mmr)
			self.filenameConfirmButton_fulldat_mmr.clicked.connect(self.loadFullDatFile_mmr)
			self.filenameConfirmButton_fulldat_mmr.setEnabled(False)

			self.filenameBrowseButton_mmdat_mmr.clicked.connect(self.chooseMMDatFile_mmr)
			self.filenameConfirmButton_mmdat_mmr.clicked.connect(self.loadMMDatFile_mmr)
			self.filenameConfirmButton_mmdat_mmr.setEnabled(False)

			self.runMMRemovalButton.clicked.connect(self.runMMRemoval)
			self.runMMRemovalButton.setEnabled(False)

			self.saveMMRemovalButton.clicked.connect(self.saveMMRemoval)
			self.saveMMRemovalButton.setEnabled(False)

			self.setPlot(tab)
	
	# ---- Macromolecule Removal ---- #
	def runMMRemoval(self):
		# 1. Fit macromolecule spectrum with HSVD.
		peak, width_L, ppm, area, phase = self.hsvd(self.MMDat_mmr,int(self.hsvdPointsLineEdit_mmr.text()),float(self.hsvdRatioLineEdit_mmr.text()),int(self.hsvdComponentsLineEdit_mmr.text()),'mm')
		hsvd_fit = Metabolite()
		hsvd_fit.peak = peak
		hsvd_fit.peak = peak
		hsvd_fit.width_L = width_L
		hsvd_fit.ppm = ppm / self.MMDat_mmr.b0
		hsvd_fit.area = area
		hsvd_fit.phase = phase
		hsvd_fid = hsvd_fit.getFID(self.MMDat_mmr.TE, self.MMDat_mmr.b0, self.MMDat_mmr.t, 0, 1, 0, 0, 0)

		self.MMDatHSVD_mmr = copy.deepcopy(self.MMDat_mmr)
		self.MMDatHSVD_mmr.signal = hsvd_fid

		ax = plt.subplot(312)
		ax.clear()
		ax.spines["top"].set_visible(False)    
		ax.spines["bottom"].set_visible(True)    
		ax.spines["right"].set_visible(False)    
		ax.spines["left"].set_visible(False)
		ax.get_xaxis().tick_bottom()

		plt.tick_params(axis="both", which="both", bottom="on", top="off",    
			labelbottom="on", left="off", right="off", labelleft="off")

		# plot raw MM signal
		f_dat, spec_dat = self.MMDat_mmr.getSpec()
		plt.plot(f_dat[0:self.MMDat_mmr.n], np.real(spec_dat[0:self.MMDat_mmr.n]), label='Data')

		# plot fitted MM signal
		f_hsvd, spec_hsvd = self.MMDatHSVD_mmr.getSpec()
		plt.plot(f_hsvd, np.real(spec_hsvd), label='Fit')

		# plot residual
		VSHIFT = 1.0
		plt.plot(f_hsvd, np.real(spec_dat[0:self.MMDat_mmr.n]) - np.real(spec_hsvd) - VSHIFT*np.amax(np.real(spec_hsvd)), label='Residual')

		# legend and title
		ax.legend(loc="upper left")
		plt.title('MM Spectrum')

		# 2. Subtract fitted macromolecule spectrum from full spectrum.
		scale = 1.3 * (float(self.fullDat_mmr.ConvS) / float(self.MMDat_mmr.ConvS))
		print self.fullDat_mmr.ConvS, self.MMDat_mmr.ConvS, (float(self.fullDat_mmr.ConvS) / float(self.MMDat_mmr.ConvS)), scale
		self.metabDat_mmr = copy.deepcopy(self.fullDat_mmr)
		self.metabDat_mmr.n = np.min([np.size(self.fullDat_mmr.signal,0), np.size(self.MMDatHSVD_mmr.signal, 0)])
		self.metabDat_mmr.signal = self.fullDat_mmr.signal[0:self.metabDat_mmr.n] - scale*self.MMDatHSVD_mmr.signal[0:self.MMDatHSVD_mmr.n]

		ax = plt.subplot(313)
		ax.clear()
		ax.spines["top"].set_visible(False)    
		ax.spines["bottom"].set_visible(True)    
		ax.spines["right"].set_visible(False)    
		ax.spines["left"].set_visible(False)
		ax.get_xaxis().tick_bottom()

		plt.tick_params(axis="both", which="both", bottom="on", top="off",    
			labelbottom="on", left="off", right="off", labelleft="off")

		f_metab, spec_metab = self.metabDat_mmr.getSpec()
		plt.plot(f_metab, np.real(spec_metab))
		plt.title('Metabolite Spectrum')

		self.canvas[1].draw()

		self.saveMMRemovalButton.setEnabled(True)

	def saveMMRemoval(self):
		# 1. Save metabolite spectrum.
		self.metabDat_mmr.filename = self.fullDat_mmr.filename.replace(self.fullDat_mmr.filename.split('/')[-1], 'sup.dat')

		out_file = open(self.metabDat_mmr.filename, 'w')
		in_file  = open(self.fullDat_mmr.filename, 'r')

		for (i, line) in enumerate(in_file):
			if i > 11:
				for element in self.metabDat_mmr.signal:
					out_file.write("{0:.6f}".format(float(np.real(element))) + '\n')
					out_file.write("{0:.6f}".format(float(np.imag(element))) + '\n')
				break
			else:
				out_file.write(line)

		out_file.close()
		in_file.close()

		# 2. Save HSVD fit.
		self.MMDatHSVD_mmr.filename = self.MMDat_mmr.filename.replace('.dat', '_hsvd.dat')
		out_file = open(self.MMDatHSVD_mmr.filename, 'w')
		in_file  = open(self.MMDat_mmr.filename, 'r')

		for (i, line) in enumerate(in_file):
			if i > 11:
				for element in self.MMDatHSVD_mmr.signal:
					out_file.write("{0:.6f}".format(float(np.real(element))) + '\n')
					out_file.write("{0:.6f}".format(float(np.imag(element))) + '\n')
				break
			else:
				out_file.write(line)

		out_file.close()
		in_file.close()

	# ---- Methods to Load Files ---- #
	def chooseFullDatFile_mmr(self):
		self.filenameConfirmButton_fulldat_mmr.setEnabled(False)
		prev = str(self.filenameInput_fulldat_mmr.text())
		self.filenameInput_fulldat_mmr.setText(str(QtWidgets.QFileDialog.getOpenFileName(self, 'Open Dat File', self.workingDirectory, 'Dat files (*.dat)')[0]))
		self.filenameConfirmButton_fulldat_mmr.setEnabled(True)
		if str(self.filenameInput_fulldat_mmr.text()) == '':
			self.filenameInput_fulldat_mmr.setText(str(prev))
			if str(prev) == '':
				self.filenameConfirmButton_fulldat_mmr.setEnabled(False)

		fullDatFile_mmr     = str(self.filenameInput_fulldat_mmr.text())
		self.workingDirectory = fullDatFile_mmr.replace(fullDatFile_mmr.split('/')[-1], '')

	def loadFullDatFile_mmr(self):
		try:
			# load dat file
			fullDatFile_mmr     = str(self.filenameInput_fulldat_mmr.text())
			self.fullDat_mmr = DatFile(fullDatFile_mmr)
			self.filenameInfoLabel_fulldat_mmr.setText(fullDatFile_mmr.split('/')[-1] + "\nsuccessfully loaded.")

			# plot
			plt.figure(2)

			ax = plt.subplot(311)
			ax.clear()
			ax.spines["top"].set_visible(False)    
			ax.spines["bottom"].set_visible(True)    
			ax.spines["right"].set_visible(False)    
			ax.spines["left"].set_visible(False)
			ax.get_xaxis().tick_bottom()

			plt.tick_params(axis="both", which="both", bottom="on", top="off",    
				labelbottom="on", left="off", right="off", labelleft="off")

			f_dat, spec_dat = self.fullDat_mmr.getSpec()
			plt.plot(f_dat[0:self.fullDat_mmr.n], np.real(spec_dat[0:self.fullDat_mmr.n]))
			plt.title('Full Spectrum')
			plt.tight_layout()
			self.canvas[1].draw()
			
			# reset buttons
			self.filenameBrowseButton_fulldat_mmr.setEnabled(True)
			self.filenameConfirmButton_fulldat_mmr.setEnabled(False)

		except Exception as e:
			self.filenameInfoLabel_fulldat_mmr.setText("ERROR: " + str(e) + "\n>> Please try again.")

	def chooseMMDatFile_mmr(self):
		self.filenameConfirmButton_mmdat_mmr.setEnabled(False)
		prev = str(self.filenameInput_mmdat_mmr.text())
		self.filenameInput_mmdat_mmr.setText(str(QtWidgets.QFileDialog.getOpenFileName(self, 'Open Dat File', self.workingDirectory, 'Dat files (*.dat)')[0]))
		self.filenameConfirmButton_mmdat_mmr.setEnabled(True)
		if str(self.filenameInput_mmdat_mmr.text()) == '':
			self.filenameInput_mmdat_mmr.setText(str(prev))
			if str(prev) == '':
				self.filenameConfirmButton_mmdat_mmr.setEnabled(False)

		MMDatFile_mmr     = str(self.filenameInput_mmdat_mmr.text())
		self.workingDirectory = MMDatFile_mmr.replace(MMDatFile_mmr.split('/')[-1], '')

	def loadMMDatFile_mmr(self):
		try:
			# load dat file
			MMDatFile_mmr     = str(self.filenameInput_mmdat_mmr.text())
			self.MMDat_mmr = DatFile(MMDatFile_mmr)
			self.filenameInfoLabel_mmdat_mmr.setText(MMDatFile_mmr.split('/')[-1] + "\nsuccessfully loaded.")

			# plot
			plt.figure(2)

			ax = plt.subplot(312)
			ax.clear()
			ax.spines["top"].set_visible(False)    
			ax.spines["bottom"].set_visible(True)    
			ax.spines["right"].set_visible(False)    
			ax.spines["left"].set_visible(False)
			ax.get_xaxis().tick_bottom()

			plt.tick_params(axis="both", which="both", bottom="on", top="off",    
				labelbottom="on", left="off", right="off", labelleft="off")

			f_dat, spec_dat = self.MMDat_mmr.getSpec()
			plt.plot(f_dat[0:self.MMDat_mmr.n], np.real(spec_dat[0:self.MMDat_mmr.n]))
			plt.title('MM Spectrum')
			self.canvas[1].draw()

			# reset buttons
			self.filenameBrowseButton_mmdat_mmr.setEnabled(True)
			self.filenameConfirmButton_mmdat_mmr.setEnabled(False)

			self.runMMRemovalButton.setEnabled(True)

		except Exception as e:
			self.filenameInfoLabel_mmdat_mmr.setText("ERROR: " + str(e) + "\n>> Please try again.")

	def setPlot(self, tab):
		if tab == 'Water Removal':
			fig = plt.figure(1)
			self.addmpl(0, fig, self.plotResult_mplvl_wr)
		elif tab == 'Macromolecule Removal':
			fig = plt.figure(2)
			self.addmpl(1, fig, self.plotResult_mplvl_mmr)

	def addmpl(self, canvas_index, fig, vertical_layout):
		self.canvas[canvas_index] = FigureCanvas(fig)
		vertical_layout.addWidget(self.canvas[canvas_index])
		self.canvas[canvas_index].draw()

		self.toolbar[canvas_index] = NavigationToolbar(self.canvas[canvas_index], self, coordinates=True)
		vertical_layout.addWidget(self.toolbar[canvas_index])

	# ---- Methods for HSVD Fitting ---- #
	def lorentzian(self, time_axis, frequency, phase, fwhm):
		oscillatory_term = np.exp(1j * (2 * np.pi * frequency * time_axis + phase))
		damping = np.exp(-time_axis * np.pi * fwhm)
		fid = oscillatory_term * damping
		return fid / len(time_axis)

	def hsvd(self, dat, n, ratio, comp, mode):
		# dat    is the dat file
		# n      is the number of points
		# ratio  is the hankel matrix row/col ratio
		# comp   is the nubmer of single related singular values
		# code based on: https://github.com/openmrslab/suspect/blob/master/suspect/processing/water_suppression.py
		
		if mode == 'mm':
			console = self.hsvdFitConsole_mmr
		elif mode == 'water':
			console = self.hsvdFitConsole_wr

		cols = n/(ratio+1)
		rows = n-cols
		L = int(np.ceil(rows))
		
		# build the Hankel matrix
		hankel_matrix = np.zeros((L, n-L+1), "complex")
		console.clear()
		console.append('Fitting:\t' + str(dat.filename))
		console.append('Creating Hankel matrix:\t[' + str(np.size(hankel_matrix, 0)) + 'x' + str(np.size(hankel_matrix, 1)) + ']')
		console.append('Points:\t' + str(n))
		console.append('Ratio:\t' + str(ratio))
		console.append('Components:\t' + str(comp))
		console.append('')
		for i in range(int(n-L)):
			hankel_matrix[:, i] = dat.signal[i:(i + L)]
		
		# perform the singular value decomposition
		U, s, V = np.linalg.svd(np.matrix(hankel_matrix))
		V = V.H # numpy returns the Hermitian conjugate of V
		
		# truncate the matrixes to the given rank (number of components)
		U_K = U[:, :comp]
		V_K = V[:, :comp]
		s_K = np.matrix(np.diag(s[:comp]))
		
		# because of the structure of the Hankel matrix, each row of U_K is the
		# result of multiplying the previous row by the delta t propagator matrix
		# Z' (a similar result holds for V as well). This gives us U_Kb * Z' = U_Kt
		# where U_Kb is U_K without the bottom row and U_Kt is U_K without the top
		# row.
		U_Kt = U_K[1:, :]
		U_Kb = U_K[:-1, :]
		
		# this gives us a set of linear equations which can be solved to find Z'.
		# Because of the noise in the system we solve with least-squares
		Zp = np.linalg.inv(U_Kb.H * U_Kb) * U_Kb.H * U_Kt
		
		# in the right basis, Zp is just the diagonal matrix describing the
		# evolution of each frequency component, by diagonalising the matrix we can
		# find that basis and get the z = exp((-damping + j*2pi * f) * dt) terms
		
		# alternatively we can just get the eigenvalues instead
		val, vec = np.linalg.eig(Zp)

		# the magnitude gives the damping and the angle gives the frequency
		damping_coeffs = np.zeros(comp)    # corresponds to width_L
		frequency_coeffs = np.zeros(comp)  # corresponds to ppm
		for i in range(comp):
			damping_coeffs[i]   = -np.log(abs(val[i])) / (1/dat.fs) / np.pi
			frequency_coeffs[i] = (np.angle(val[i]) / ((1/dat.fs) * 2 * np.pi))
		
		# we can calculate the magnitude of each signal from the
		# RHS decomposition, linalg.inv(vec) * (S_K * V_K.H)[:, 0] but
		# a simpler but more expensive way is to construct a basis set from the
		# known damping and frequency components and fit to the original data to
		# get the amplitudes and phase data
		X = np.zeros((dat.n, comp), "complex")
		# TODO this should use the singlet fitting module to make the basis
		for i in range(comp):
			X[:, i] = self.lorentzian(dat.t,
								      frequency_coeffs[i],
								      0,
								      damping_coeffs[i]) * dat.n

		# we use the linear non-iterative least squares again
		U2, s2, V2 = np.linalg.svd(np.matrix(X), full_matrices=False)
		s2_inv = np.diag(1 / s2)
		beta = V2.H * s2_inv * U2.H * np.matrix(np.reshape(dat.signal[0:dat.n], (dat.n, 1)))
		amplitudes = np.squeeze(np.array(np.abs(beta)))
		phases     = np.squeeze(np.rad2deg(np.array(np.angle(beta))))
		
		amplitudes = [x for _,x in sorted(zip(frequency_coeffs,amplitudes))]
		damping_coeffs = [x for _,x in sorted(zip(frequency_coeffs,damping_coeffs))]
		phases = [x for _,x in sorted(zip(frequency_coeffs,phases))]
		frequency_coeffs = sorted(frequency_coeffs)
		
		console.append('peak \t feq \t ampl \t damp \t phase')
		for i in range(0,comp):
			console.append(str(i) + '\t' + str(frequency_coeffs[i]) + '\t' + str(amplitudes[i]) + '\t' + str(damping_coeffs[i]) + '\t' + str(phases[i]))
		
		return np.array(range(0, comp)), np.array(damping_coeffs), np.array(frequency_coeffs), np.array(amplitudes), np.array(phases)

# ---- Launch Application ---- #
if __name__ == "__main__":
	app = QtWidgets.QApplication(sys.argv)
	window = MyApp()
	window.show()
	sys.exit(app.exec_())