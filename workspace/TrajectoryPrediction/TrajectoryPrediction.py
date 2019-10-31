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
from numpy import float32

sys.path.append(".")

# Import RTM module
import RTC
import OpenRTM_aist


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

		for i in range(11):
			self.dummy = str(i+11) + " " + str(1) + " ? ? "
			self.dummylist.append(self.dummy)
		print(self.dummylist)
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
		src = "./data/test/crowds/input.txt"
		copy = "./data/test/crowds/file01.txt"
		dataBuffer = []
		
		# 人の座標を10フレームごとinputファイルに書きこむ
		if self._HumanPointIn.isNew():

			self._d_HumanPoint = self._HumanPointIn.read()
			
			humanpoint_x = self._d_HumanPoint.data[0]
			humanpoint_y = self._d_HumanPoint.data[1]
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
					# for ele in self.HumanPointList:
					# 	f.write(ele + '\n')
				self.HumanPointList = []
				shutil.copyfile(src,copy)

				print("Predict...........")
				result = test.main()	
				# tensorList⇒numpy.float
				result = result.numpy()

				# numpy.float⇒float
				for i in range(5):
					HumanPredictionPoint_x = float(result[i,0,0])
					HumanPredictionPoint_y = float(result[i,0,1])
					dataBuffer.append(HumanPredictionPoint_x)
					dataBuffer.append(HumanPredictionPoint_y)

				self._d_PredictionHumanPoint.data = dataBuffer
				print("write")
				print(dataBuffer)
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
	mgr = OpenRTM_aist.Manager.init(sys.argv)
	mgr.setModuleInitProc(MyModuleInit)
	mgr.activateManager()
	mgr.runManager()

if __name__ == "__main__":
	main()

