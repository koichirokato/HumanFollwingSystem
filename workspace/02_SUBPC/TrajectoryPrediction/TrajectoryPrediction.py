#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -*- Python -*-

"""
 @file TrajectoryPrediction.py
 @brief ModuleDescription
 @date $Date$


"""
import sys
import time
import torch
import test
import shutil
import os
from numpy import float32
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import tkinter as tk
import datetime
import csv

from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.figure import Figure
import matplotlib.animation as animation
import math

sys.path.append(".")

# Import RTM module
import RTC
import OpenRTM_aist

# global
i = 0
humanpoint_x, humanpoint_y = 0.0, 0.0
result_plot = np.array([0,0,0])
plot_flag = 0

# Import Service implementation class
# <rtc-template block="service_impl">

# </rtc-template>

# Import Service stub modules
# <rtc-template block="consumer_import">
# </rtc-template>


# This module's spesification
# <rtc-template block="module_spec">
trajectoryprediction_spec = ["implementation_id", "TrajectoryPrediction", 
		 "type_name",         "TrajectoryPrediction", 
		 "description",       "ModuleDescription", 
		 "version",           "1.0.0", 
		 "vendor",            "kato", 
		 "category",          "Category", 
		 "activity_type",     "STATIC", 
		 "max_instance",      "1", 
		 "language",          "Python", 
		 "lang_type",         "SCRIPT",
		 ""]
# </rtc-template>

##
# @class TrajectoryPrediction
# @brief ModuleDescription
# 
# 
class TrajectoryPrediction(OpenRTM_aist.DataFlowComponentBase):
	
	##
	# @brief constructor
	# @param manager Maneger Object
	# 
	def __init__(self, manager):
		OpenRTM_aist.DataFlowComponentBase.__init__(self, manager)

		self._d_HumanPoint = RTC.TimedDoubleSeq(RTC.Time(0,0), [])
		"""
		"""
		self._HumanPointIn = OpenRTM_aist.InPort("HumanPoint", self._d_HumanPoint)
		self._d_PredictionHumanPoint = RTC.TimedDoubleSeq(RTC.Time(0,0), [10])
		"""
		"""
		self._PredictionHumanPointOut = OpenRTM_aist.OutPort("PredictionHumanPoint", self._d_PredictionHumanPoint)


		


		# initialize of configuration-data.
		# <rtc-template block="init_conf_param">
		
		# </rtc-template>


		 
	##
	#
	# The initialize action (on CREATED->ALIVE transition)
	# formaer rtc_init_entry() 
	# 
	# @return RTC::ReturnCode_t
	# 
	#
	def onInitialize(self):
		# Bind variables and configuration variable
		
		# Set InPort buffers
		self.addInPort("HumanPoint",self._HumanPointIn)
		
		# Set OutPort buffers
		self.addOutPort("PredictionHumanPoint",self._PredictionHumanPointOut)
		
		# Set service provider to Ports
		
		# Set service consumers to Ports
		
		# Set CORBA Service Ports
		

		self.cnt        = 0
		self.result     = []
		self.dummylist  = []
		self.dummy      = ""
		self.HumanPointList = []
		self.i = 0
		self.fflag = 1
		for i in range(11):
			self.dummy = str(i+11) + " " + str(1) + " ? ? "
			self.dummylist.append(self.dummy)
		# print(self.dummylist)

		return RTC.RTC_OK
	
	###
	## 
	## The finalize action (on ALIVE->END transition)
	## formaer rtc_exiting_entry()
	## 
	## @return RTC::ReturnCode_t
	#
	## 
	#def onFinalize(self):
	#
	#	return RTC.RTC_OK
	
	###
	##
	## The startup action when ExecutionContext startup
	## former rtc_starting_entry()
	## 
	## @param ec_id target ExecutionContext Id
	##
	## @return RTC::ReturnCode_t
	##
	##
	#def onStartup(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	###
	##
	## The shutdown action when ExecutionContext stop
	## former rtc_stopping_entry()
	##
	## @param ec_id target ExecutionContext Id
	##
	## @return RTC::ReturnCode_t
	##
	##
	#def onShutdown(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	##
	#
	# The activated action (Active state entry action)
	# former rtc_active_entry()
	#
	# @param ec_id target ExecutionContext Id
	# 
	# @return RTC::ReturnCode_t
	#
	#
	def onActivated(self, ec_id):
		print("Activate")
		return RTC.RTC_OK
	
	##
	#
	# The deactivated action (Active state exit action)
	# former rtc_active_exit()
	#
	# @param ec_id target ExecutionContext Id
	#
	# @return RTC::ReturnCode_t
	#
	#
	def onDeactivated(self, ec_id):
		print("Deactivate")
		return RTC.RTC_OK
	
	##
	#
	# The execution action that is invoked periodically
	# former rtc_active_do()
	#
	# @param ec_id target ExecutionContext Id
	#
	# @return RTC::ReturnCode_t
	#
	#
	
	def onExecute(self, ec_id):
		# inputファイル、予測ファイル
		# print("execute")
		src  = "./data/test/crowds/input.txt"
		copy = "./data/test/crowds/file01.txt"
		dataBuffer = []
		os.chmod(src, 0o666)
		
		# 人の座標を10フレームごとinputファイルに書きこむ
		if self._HumanPointIn.isNew():
			if (self.fflag == 1) and (self.cnt == 0):
				self.fflag = 0
			else:
				self._d_HumanPoint = self._HumanPointIn.read()

				global humanpoint_x, humanpoint_y
				humanpoint_x = self._d_HumanPoint.data[0]
				humanpoint_y = self._d_HumanPoint.data[1]
				if humanpoint_x > 1000000000000000000000:
					return RTC.RTC_OK
				self.cnt += 1
				
				# 1行の文字列にする、これが1フレーム
				s = str(self.cnt)+ " " + str(1) + " " + str(humanpoint_y) + " " + str(humanpoint_x)
				
				self.HumanPointList.append(s)
				# 10行ごとに追加、予測、出力
				if(self.cnt>=10):
					self.cnt = 0
					for ele in self.dummylist:
						self.HumanPointList.append(ele)
					
					with open(src, mode='w') as f:
						f.write('\n'.join(self.HumanPointList))

					self.HumanPointList = []
					shutil.copyfile(src,copy)

					print("Predict...........")
					result = test.main()	
					# tensorList⇒numpy.float
					result = result.numpy()

					global result_plot
					result_plot = result
					
					# numpy.float⇒float
					for i in range(10):
						HumanPredictionPoint_x = float(result[i,0,0])
						HumanPredictionPoint_y = float(result[i,0,1])
						dataBuffer.append(HumanPredictionPoint_x)
						dataBuffer.append(HumanPredictionPoint_y)

					print(dataBuffer)
					self._d_PredictionHumanPoint.data = dataBuffer
					print("write")

					global plot_flag
					plot_flag = 1
					
					self._PredictionHumanPointOut.write()
		
		return RTC.RTC_OK
	
	###
	##
	## The aborting action when main logic error occurred.
	## former rtc_aborting_entry()
	##
	## @param ec_id target ExecutionContext Id
	##
	## @return RTC::ReturnCode_t
	##
	##
	#def onAborting(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	###
	##
	## The error action in ERROR state
	## former rtc_error_do()
	##
	## @param ec_id target ExecutionContext Id
	##
	## @return RTC::ReturnCode_t
	##
	##
	#def onError(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	###
	##
	## The reset action that is invoked resetting
	## This is same but different the former rtc_init_entry()
	##
	## @param ec_id target ExecutionContext Id
	##
	## @return RTC::ReturnCode_t
	##
	##
	#def onReset(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	###
	##
	## The state update action that is invoked after onExecute() action
	## no corresponding operation exists in OpenRTm-aist-0.2.0
	##
	## @param ec_id target ExecutionContext Id
	##
	## @return RTC::ReturnCode_t
	##

	##
	#def onStateUpdate(self, ec_id):
	#
	#	return RTC.RTC_OK
	
	###
	##
	## The action that is invoked when execution context's rate is changed
	## no corresponding operation exists in OpenRTm-aist-0.2.0
	##
	## @param ec_id target ExecutionContext Id
	##
	## @return RTC::ReturnCode_t
	##
	##
	#def onRateChanged(self, ec_id):
	#
	#	return RTC.RTC_OK

def TrajectoryPredictionInit(manager):
    profile = OpenRTM_aist.Properties(defaults_str=trajectoryprediction_spec)
    manager.registerFactory(profile,
                            TrajectoryPrediction,
                            OpenRTM_aist.Delete)

def MyModuleInit(manager):
    TrajectoryPredictionInit(manager)

    # Create a component
    comp = manager.createComponent("TrajectoryPrediction")

def main():
	# make log file(csv)
	now = datetime.datetime.now()
	logfile = './log_csv/log_' + now.strftime('%Y%m%d_%H%M%S') + '.csv'
	open(logfile, mode='w')
	mgr = OpenRTM_aist.Manager.init(sys.argv)
	mgr.setModuleInitProc(MyModuleInit)
	mgr.activateManager()
	mgr.runManager(True)

	def _quit():
		root.quit()     # stops mainloop
		root.destroy()  # this is necessary on Windows to prevent
						# Fatal Python Error: PyEval_RestoreThread: NULL tstate
	
	# plot Humanpoint and PredictionHumanPoint using Tk
	def plot():
		sns.set()
		global humanpoint_x, humanpoint_y, result_plot
		prediction_humanpoint_x, prediction_humanpoint_y, logtmp = [], [], []
		tmp = ''
			
		global plot_flag
		if plot_flag == 1:
			print("plot prediction")
			for i in range(10):
				prediction_humanpoint_x.append(float(result_plot[i,0,0]))
				prediction_humanpoint_y.append(float(result_plot[i,0,1]))
		
			plt.scatter(prediction_humanpoint_x, prediction_humanpoint_y, c='red')

			# log
			with open(logfile, mode='a') as f:
				writer = csv.writer(f, lineterminator='\n')
				for i in range(10):
					tmp = ''
					tmp = str(prediction_humanpoint_x[i])
					logtmp.append(tmp)
					tmp = str(prediction_humanpoint_y[i])
					logtmp.append(tmp)
					writer.writerow(logtmp)
					
			plot_flag = 0

		plt.scatter(humanpoint_x, humanpoint_y, c='blue')

		# draw => root.after
		canvas.draw()
		root.after(100,plot)


	root = tk.Tk()
	root.title("Trajectory")

	fig, ax = plt.subplots(1, 1)
	# FuncAnimationより前に呼ぶ必要がある
	canvas = FigureCanvasTkAgg(fig, master=root)  # A tk.DrawingArea.
	ax.set_xlim((-10, 10))
	ax.set_ylim((-10, 10))

	toolbar = NavigationToolbar2Tk(canvas, root)
	canvas.get_tk_widget().pack()

	button = tk.Button(master=root, text="Quit", command=_quit)
	
	# plot loop
	root.after(100, plot)
	button.pack()
	
	root.mainloop()
	

if __name__ == "__main__":
	main()

