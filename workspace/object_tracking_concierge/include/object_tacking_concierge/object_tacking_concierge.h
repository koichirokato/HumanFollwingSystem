// -*- C++ -*-
/*!
 * @file  object_tacking_concierge.h
 * @brief ModuleDescription
 * @date  $Date$
 *
 * $Id$
 */

#ifndef OBJECT_TACKING_CONCIERGE_H
#define OBJECT_TACKING_CONCIERGE_H

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>
#include "InterfaceDataTypesStub.h"
#include "BasicDataTypeStub.h"
#include "ExtendedDataTypesStub.h"


// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="port_stub_h">
// </rtc-template>

using namespace RTC;

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>

#include "math.h"
#include <stdio.h>
#include <sys/types.h>
#include <process.h>
#include <windows.h>

#define PI 3.14159265359

/*!
 * @class object_tacking_concierge
 * @brief ModuleDescription
 *
 */
class object_tacking_concierge
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  object_tacking_concierge(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~object_tacking_concierge();

  // <rtc-template block="public_attribute">
  
  // </rtc-template>

  // <rtc-template block="public_operation">
  
  // </rtc-template>

  /***
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry() 
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onInitialize();

  /***
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onFinalize();

  /***
   *
   * The startup action when ExecutionContext startup
   * former rtc_starting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  /***
   *
   * The shutdown action when ExecutionContext stop
   * former rtc_stopping_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  /***
   *
   * The activated action (Active state entry action)
   * former rtc_active_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  /***
   *
   * The deactivated action (Active state exit action)
   * former rtc_active_exit()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   *
   * The execution action that is invoked periodically
   * former rtc_active_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

  /***
   *
   * The aborting action when main logic error occurred.
   * former rtc_aborting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  /***
   *
   * The error action in ERROR state
   * former rtc_error_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   *
   * The reset action that is invoked resetting
   * This is same but different the former rtc_init_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
  /***
   *
   * The state update action that is invoked after onExecute() action
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  /***
   *
   * The action that is invoked when execution context's rate is changed
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);


 protected:
	 short int m_follow_distance;
	 int m_va_value;
	 int m_vx_value;
	 int robot_mode;
	 int voice;
	 
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">

  RTC::RangeData m_RangeData;
  /*!
   */
  InPort<RTC::RangeData> m_RangeDataIn;
  // </rtc-template>

//### 20161210 K.Suzuki ###
  TimedString m_user_info;

  InPort<TimedString> m_user_infoIn;
  
  TimedPose2D m_RobotPose2D;
  /*!
   */
  InPort<TimedPose2D> m_RobotPose2DIn;
  RTC::TimedDoubleSeq m_PredictionHumanPoint;

  InPort<RTC::TimedDoubleSeq> m_PredictionHumanPointIn;
  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::TimedVelocity2D m_black_ship;
  /*!
   */
  OutPort<RTC::TimedVelocity2D> m_black_shipOut;
  /*!
   */
  RTC::TimedDoubleSeq m_HumanPoint;
  /*!
   */
  OutPort<RTC::TimedDoubleSeq>m_HumanPointOut;
  
  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  
  // </rtc-template>

 private:
	 double x,y,r,ang,diff;
	 double position[2], last_position[2];
	 void Voice_Output(int time,char str[],bool display);
	 bool time_Move(int time,double x,double y,double a);
	 bool just_Move(double x,double y,double a);
	 bool flag;
	 int m_target_diff;
	 void urg_read();
	 double delta_x, delta_y;
	 void crawler(int time, int command);
	 int status_check();
	 int timer;
	 bool open_check;
	 bool log_open();
	 bool log_close();
	 void wait(int time);
	 void CppSpline_x(const double x[]);
	 void CppSpline_y(const double y[]);
	 //double potential_route_vx(double x, double y, double obst_x, double obst_y, double goal_x, double goal_y);
	 void potential_route(double x, double y, double obst_x, double obst_y, double goal_x, double goal_y, double potential_result[2]);
	 //double potential_route_va(double x, double y, double obst_x, double obst_y, double goal_x, double goal_y);
	 double potential_field(double x, double y, double obst_x, double obst_y, double goal_x, double goal_y);
	 int shape_discrimination(int stid, int edid, int mdid,int obj, double start_obj_point_x, double start_obj_point_y, double end_obj_point_x, double end_obj_point_y);
	 double calc_x(double t);
	 double calc_y(double t);

	 void Feature_quantity(int id);

	 int real_count;
	 int real_count_1;
	 int roop;
	 int avoid;

	 double shoulder_length_squaring_x;
	 double shoulder_length_squaring_y;
	 double shoulder_length_squaring_z;
	 double Right_Head_squaring_x;
	 double Right_Head_squaring_y;
	 double Right_Head_squaring_z;
	 double left_Head_squaring_x;
	 double left_Head_squaring_y;
	 double left_Head_squaring_z;
	 double shoulder_length[200];
	 double Right_Head_length[200];
	 double Left_Head_length[200];
	 //### 20170911 Shimoyama
	 double Right_Head_length_sum[2];
	 double Right_Head_length_ave[2];
	 double theta_right[200];
	 double theta_right_sum[2];
	 double theta_right_ave[2];

	 //####20180117 Shimoyama
	 double torso_dim[2];
	 //double Centroid_x[600];
	 //double Centroid_y[600];
	 
	 double ax_[3];
	 double bx_[3];
	 double cx_[3];
	 double dx_[3];
	 double wx_[3];

	 double ay_[3];
	 double by_[3];
	 double cy_[3];
	 double dy_[3];
     double wy_[3];


	 double left_shoulder_point_x[200];
	 double left_shoulder_point_y[200];
	 double right_shoulder_point_x[200];
	 double right_shoulder_point_y[200];
	 double head_point_x[2];
	 double head_point_y[2];
	 double head_point_z[200];
	 double right_shoulder_y[200];
	 double shoulder_length_sum[2];
	 double right_shoulder_y_sum[2];
	 double head_point_z_sum[2];
	 //double head_point_y_sum[2];
	 //double Centroid_x_sum[2];
	 //double Centroid_y_sum[2];
	 double shoulder_length_ave[2]; 
	 double right_shoulder_y_ave[2];
	 double head_point_z_ave[2];
	 //double head_point_y_ave[2];
	 //double Centroid_x_ave[2];
	 //double Centroid_y_ave[2];
	 double head_height[2];
	 double y1_head;
	 double y2_head;
	 double y1_shoulder;
	 double y2_shoulder;
	 double b_head;
	 double b_shoulder;
	 int find_character;
	 int tracking_flag;
	
	 std::string str; //ˆÄ“à—p


  // <rtc-template block="private_attribute">
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void object_tacking_conciergeInit(RTC::Manager* manager);
};

#endif // OBJECT_TACKING_CONCIERGE_H
