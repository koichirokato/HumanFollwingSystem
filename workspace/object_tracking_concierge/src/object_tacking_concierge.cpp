// -*- C++ -*-
/*!
 * @file  object_tacking_concierge.cpp
 * @brief ModuleDescription
 * @date $Date$ 20170908 Shimoyama�@�����ʂ��g�����Ǐ]�ɕύX
 * ������:�@�Ƃ邻�̖ʐρ�2500 
 * $Id$
 */
 //

#include "object_tacking_concierge.h"
#include <Windows.h>
#include <tchar.h>
#include <math.h>
#include <cassert>
//### 20161210 K.Suzuki ###
#define NEW_TRACKING

typedef struct {
	int id;
//	float head_x, head_y, head_z;
	float torso_x, torso_y, torso_z;
	float right_hand_x, right_hand_y, right_hand_z;
	float right_elbow_x, right_elbow_y, right_elbow_z;
	float right_shoulder_x, right_shoulder_y, right_shoulder_z;
} sUSER_INFO;

typedef struct {
	int num;	// Number of users
	int target;	// Target user id
	float target_x, target_y, target_z;
	float target_old_x, target_old_y, target_old_z;
	sUSER_INFO user_info[15];
} sUSER_INFOS;

sUSER_INFOS user_infos;

int momlost_flg;
//### 20161210 K.Suzuki ###

#define MAX_RANGE 2000

// �v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
int WINAPI window_create(HINSTANCE hInst);
int draw_window();

double g_kinect_data_x=0.0,g_kinect_data_z=0.0;
double last_kinect_x=9999,last_kinect_z=9999;
HINSTANCE Inst;
WNDCLASSEX wc;
HWND hWnd;
HDC hdc;
HDC memDC;
MSG msg;

double scan_data[800]; //long -> double
int target_angle=0;
int start[150]={0},end[150]={0};
int count_check[6];
double object_point[100][2]={0};
double object[2]={0};
double object_size[100]={0};
double human_point[150]={0};
double last_human_point[2]={0};
double human_dist=0;
double lock_point[2]={0,1000};
double human_urg[2]={0};
double a=0,theta=0;
int rock=500;
double urg_ang = 0.0;
double urg_x[680] = {0};
double urg_y[680] = {0};
int right_counter = 0, center_counter = 0, left_counter = 0;
double va = 0, vx = 0, prevX = 0;
double vx_value=0.7;

FILE *fp;

FILE *fplog = NULL;	//### 20160725 K.Suzuki : log file

// Module specification
// <rtc-template block="module_spec">
static const char* object_tacking_concierge_spec[] =
  {
    "implementation_id", "object_tacking_concierge",
    "type_name",         "object_tacking_concierge",
    "description",       "ModuleDescription",
    "version",           "1.0.0",
    "vendor",            "Kazuma_Fujimoto",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
	"conf.default.follow_distance", "1000",
	"conf.default.va_value", "9",
	"conf.default.vx_value", "9",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
object_tacking_concierge::object_tacking_concierge(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_RangeDataIn("RangeData", m_RangeData),
    m_RobotPose2DIn("RobotPose2D", m_RobotPose2D),
    m_black_shipOut("black_ship", m_black_ship),
    m_user_infoIn("user_info", m_user_info),
	m_PredictionHumanPointIn("PredictionHumanPoint", m_PredictionHumanPoint),
	m_HumanPointOut("HumanPoint",m_HumanPoint)
    // </rtc-template>
{
	//### 20160725 K.Suzuki : log file open
	time_t timer;
	struct tm *t_st;
	time(&timer);
	t_st = localtime(&timer);
	char filename[50];
	sprintf(filename,"log\\%04d_%02d%02d_%02d%02d%02d.txt" ,t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec);
	if ((fplog = fopen(filename, "w")) == NULL) {
		printf("### Cannot open %s ###\n", filename);
	}
}

/*!
 * @brief destructor
 */
object_tacking_concierge::~object_tacking_concierge()
{
	//### 20160725 K.Suzuki : log file open
	if(fplog != NULL) fclose(fplog);
}



RTC::ReturnCode_t object_tacking_concierge::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("RangeData", m_RangeDataIn);
  m_RangeData.ranges.length(800);
  addInPort("user_info", m_user_infoIn);
  addInPort("RobotPose2D",m_RobotPose2DIn);
  addInPort("PredictionHumanPoint",m_PredictionHumanPointIn);
  // Set OutPort buffer
  addOutPort("black_ship", m_black_shipOut);
  addOutPort("humanpoint",m_HumanPointOut);
  m_HumanPoint.data.length(2);
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  bindParameter("folow_distance", m_follow_distance, "1000");
  bindParameter("va_value", m_va_value, "3");
  bindParameter("vx_value", m_vx_value, "0.7");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t object_tacking_concierge::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t object_tacking_concierge::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t object_tacking_concierge::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/
RTC::ReturnCode_t object_tacking_concierge::onActivated(RTC::UniqueId ec_id)
{
  timer = 0;
  x = 0.0; y = 0.0; r = 0.0;	//value initialization
  ang = 0.0;
  diff = 0.0;
  int n = 0;
  avoid = 0;
  voice = 0;
  for(int i = 0; i<2; i++){
	position[i] = 0.0;
	last_position[i] = 0.0;
  }
  flag = true;
  Inst=GetModuleHandle(NULL);
	window_create(Inst);
	BOOL ret2 = GetMessage( &msg, NULL, 0, 0 );  // ���b�Z�[�W���擾����
	if( ret2 == 0 || ret2 == -1 )
	{
		// �A�v���P�[�V�������I�������郁�b�Z�[�W�����Ă�����A
		// ���邢�� GetMessage() �����s������( -1 ���Ԃ��ꂽ�� �j�A���[�v�𔲂���
	}
	else
	{
		// ���b�Z�[�W����������
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	//open_check = false;
	log_open();
	just_Move(0, 0, 0);
	Sleep(2000);
	std::cout<<"activate"<<std::endl;

	//### 20161210 K.Suzuki ###
	// Initialize
	user_infos.num = 0;
	user_infos.target = (-1);
	user_infos.target_old_x = user_infos.target_old_y = user_infos.target_old_z = 0.0f;
	momlost_flg = 0;
  return RTC::RTC_OK;
}


RTC::ReturnCode_t object_tacking_concierge::onDeactivated(RTC::UniqueId ec_id)
{
	//if(open_check == true){
	log_close();
	just_Move(0, 0, 0);
	//}
  return RTC::RTC_OK;
}

double pre_x=0.0,pre_z=0.0;
short sw=0;
bool stay_flag          = 1;			//### 20160725 K.Suzuki : �N�����X�e�C���
bool obst_flag          = 0;
bool avoid_flag         = 0;
bool recovery_flag      = 0;			//�����̕��A�t���O
bool right_hand_up_flag = 0;			//### 20160725 K.Suzuki : ��ԕω������o���邽��
bool slow_flag          = 0;
bool first_time         = 1;			//### 20160725 K.Suzuki : �f���J�n����A�ŏ��ɉE�肪������܂ł�1
int count_log = 0;

double potential_vx = 0;
double potential_va = 0;
int no_update_count = 0, predict_count = 0;
double prediction_human_point_x[10] = {0};
double prediction_human_point_y[10] = {0};
double prediction_log_x, prediction_log_y;

RTC::ReturnCode_t object_tacking_concierge::onExecute(RTC::UniqueId ec_id)
{
	std::cout<<"user_infos.target = "<<user_infos.target<<std::endl;
	std::cout<<"human_urg_X = "<<human_urg[0]<<"human_urg_Y = "<<human_urg[1]<<std::endl;
	std::cout<<"human_point_X = "<<human_point[0]<<"human_point_Y = "<<human_point[1]<<std::endl;
	std::cout<<"prediction_x = "<<prediction_human_point_x[0]<<";  prediction_y = "<<prediction_human_point_y[0]<<std::endl;
	std::cout<<"lock_point_X = "<<lock_point[0]<<"lock_point_Y = "<<lock_point[1]<<std::endl;
	
	if(fplog != NULL) fprintf(fplog, "onExecute\n");	//### 20160725 K.Suzuki : log

//### 20161210 K.Suzuki ###
#ifdef NEW_TRACKING
	//###################
	//### Kinect Data ###
	//###################
	//### Multiple users.
	int user_info_get = 0;
	while(1){	// Read the latest data.
		int print_sw = 0;	// Local report sw.
		if(m_user_infoIn.isNew()){
			m_user_infoIn.read();
			std::string msg = m_user_info.data;
			if(print_sw) std::cout<<msg<<std::endl;
			user_info_get = 1;	// New data was read.

			char buf[4096];
			sprintf(buf, "%s", msg.c_str());
			sscanf(buf, "NUM=%d", &user_infos.num);
			if(print_sw) printf("NUM=%d\n", user_infos.num);
			int j = 0;
			for(int i=0; i<user_infos.num; i++){
				int buf_len = strlen(buf);
				for(; j<buf_len; j++){
					if(strncmp(&buf[j], "USER_ID", 7)) continue;
					sscanf(&buf[j], "USER_ID(%d)", &user_infos.user_info[i].id);
					if(print_sw) printf("USER_ID(%d)\n", user_infos.user_info[i].id);
					break;
				}
				for(; j<buf_len; j++){
					if(strncmp(&buf[j], "TORSO", 5)) continue;
					sscanf(&buf[j], "TORSO(%f,%f,%f)", &user_infos.user_info[i].torso_x, &user_infos.user_info[i].torso_y, &user_infos.user_info[i].torso_z);
					if(print_sw) printf("TORSO(%f,%f,%f)\n", user_infos.user_info[i].torso_x, user_infos.user_info[i].torso_y, user_infos.user_info[i].torso_z);
					break;
				}
				for(; j<buf_len; j++){
					if(strncmp(&buf[j], "RIGHT_HAND", 10)) continue;
					sscanf(&buf[j], "RIGHT_HAND(%f,%f,%f)", &user_infos.user_info[i].right_hand_x, &user_infos.user_info[i].right_hand_y, &user_infos.user_info[i].right_hand_z);
					if(print_sw) printf("RIGHT_HAND(%f,%f,%f)\n", user_infos.user_info[i].right_hand_x, user_infos.user_info[i].right_hand_y, user_infos.user_info[i].right_hand_z);
					break;
				}
				for(; j<buf_len; j++){
					if(strncmp(&buf[j], "RIGHT_ELBOW", 11)) continue;
					sscanf(&buf[j], "RIGHT_ELBOW(%f,%f,%f)", &user_infos.user_info[i].right_elbow_x, &user_infos.user_info[i].right_elbow_y, &user_infos.user_info[i].right_elbow_z);
					if(print_sw) printf("RIGHT_ELBOW(%f,%f,%f)\n", user_infos.user_info[i].right_elbow_x, user_infos.user_info[i].right_elbow_y, user_infos.user_info[i].right_elbow_z);
					break;
				}
				for(; buf_len; j++){
					if(strncmp(&buf[j], "RIGHT_SHOULDER", 14)) continue;
					sscanf(&buf[j], "RIGHT_SHOULDER(%f,%f,%f)", &user_infos.user_info[i].right_shoulder_x, &user_infos.user_info[i].right_shoulder_y, &user_infos.user_info[i].right_shoulder_z);
					if(print_sw) printf("RIGHT_SHOULDER(%f,%f,%f)\n", user_infos.user_info[i].right_shoulder_x, user_infos.user_info[i].right_shoulder_y, user_infos.user_info[i].right_shoulder_z);
					break;
				}
			}
			continue;
		}
		break;
	}

#if 0

#endif

	//##########################
	//### �E��ɂ�鑀��     ###
	//### Right hand gesture ###
	//##########################
	if(user_infos.num > 0){
		int right_hand_up_detect = 0;
		sUSER_INFO *pui = user_infos.user_info;;
		for(int i=0; i < user_infos.num; i++, pui++){
			if(pui->torso_z > 2200) continue;
			if(pui->right_hand_y > pui->right_elbow_y && pui->right_elbow_y > pui->right_shoulder_y){
				right_hand_up_detect = 1;
				momlost_flg = 0;
				user_infos.target = pui->id;	// Set the target user id.
				user_infos.target_old_x = user_infos.target_x = pui->torso_x;
				user_infos.target_old_y = user_infos.target_y = pui->torso_y;
				user_infos.target_old_z = user_infos.target_z = pui->torso_z;
				printf("\n\n\n### RIGHT HAND UP ###\n\n\n\n");

				break;
			}
		}
		if(right_hand_up_detect){
			//### �E�肪�E�Ђ�����ł��E�Ђ����E������ɂ���ΒǏ]���ꎞ��~         ###
			//### When right hand > right elbow > right shoulder, robot will be suspended. ###
			//### 1��̋��薈�ɃX�C�b�`��ON/OFF���݂ɐ؂�ւ���
			//### flip-flop
			if(right_hand_up_flag == 0){//ON
				//if(abs(m_kinect_head.data.z-2000)<100){//2000�t�߂ɐl������Ƃ�
					if(stay_flag == 1){
						stay_flag = 0;
						Beep(4000, 100);
						Beep(8000, 100);
						Beep(4000, 100);
						tracking_flag = 1;
						if(fplog != NULL) fprintf(fplog, "stay_flag = 0\n");	//### 20160725 K.Suzuki : log 
						//Sleep(3000);
					}
					else
					{//OFF
						stay_flag = 1;
						Beep(4000, 500);
						if(fplog != NULL) fprintf(fplog, "stay_flag = 1\n");	//### 20160725 K.Suzuki : log 
						//### �����I�ɒ�~
						just_Move(0, 0, 0);
						tracking_flag  = 0;
						find_character = 0;
					}
				//}
				right_hand_up_flag = 1;
				if(first_time){
					first_time = 0;
				}
			}
		}
		else
		{
			right_hand_up_flag = 0;
			//### Momental lost
			if(user_infos.target >= 0){
				sUSER_INFO *pui = user_infos.user_info;;
				for(int i=0; i<user_infos.num; i++, pui++){
					if(pui->id != user_infos.target) continue;
					user_infos.target_x = pui->torso_x;
					user_infos.target_y = pui->torso_y;
					user_infos.target_z = pui->torso_z;
					break;
				}
				float dx = (float)fabs((double)(user_infos.target_x - user_infos.target_old_x));
				float dy = (float)fabs((double)(user_infos.target_y - user_infos.target_old_y));
				float dz = (float)fabs((double)(user_infos.target_z - user_infos.target_old_z));
				if(dx > 800 || dy > 800 || dz > 800){
					// Momental lost
					user_infos.target = (-1);
					momlost_flg = 1;
					printf("\n\n\n###<>###<>### MOMENTAL LOST ###<>###()###\n\n\n\n");
				}
				else
				{
					// Update backuped location
					user_infos.target_old_x = user_infos.target_x;
					user_infos.target_old_y = user_infos.target_y;
					user_infos.target_old_z = user_infos.target_z;
				}
			}
			else
			{
//				if(momlost_flg){
					sUSER_INFO *pui = user_infos.user_info;;
					for(int i=0; i<user_infos.num; i++, pui++){
						float dx = (float)fabs((double)(pui->torso_x - user_infos.target_old_x));
						float dy = (float)fabs((double)(pui->torso_y - user_infos.target_old_y));
						float dz = (float)fabs((double)(pui->torso_z - user_infos.target_old_z));
						if(dx <= 800 && dy <= 800 && dz <= 800){
							momlost_flg = 0;
							user_infos.target = pui->id;	// Set the target user id.
							user_infos.target_old_x = user_infos.target_x = pui->torso_x;
							user_infos.target_old_y = user_infos.target_y = pui->torso_y;
							user_infos.target_old_z = user_infos.target_z = pui->torso_z;
							printf("\n\n\n***<>***<>*** RECOVERY ***<>***()***\n\n\n\n");
						
							break;
						}
					}
//				}
			}
		}
	}
#endif //NEW_TRACKING

	if(first_time){
		// �ǂݔ�΂�
		if(m_RangeDataIn.isNew()) m_RangeDataIn.read();
		goto END;
	}

	//########################################
	//### �����W�f�[�^�ƃg���\�f�[�^�̏��� ###
	//### Range data and torso data        ###
	//########################################
	if(m_RangeDataIn.isNew()){
		if(fplog != NULL) fprintf(fplog, "  m_RangeDataIn : ");	//### 20160725 K.Suzuki : log 
		human_dist    = 9999;
		double mini_dist = 9999;  //int -> double
		double mini_heading = 0;
		m_RangeDataIn.read();
		for(int scan_roop = 44; scan_roop <= 724; scan_roop++)
		{
			//### 20�����A�܂���2000�����̋�����0�ɗ��Ƃ� ###
			//### Distance limitation : 20�`2000mm     ###
			if(m_RangeData.ranges[scan_roop] > MAX_RANGE)m_RangeData.ranges[scan_roop] = 0;
			if(m_RangeData.ranges[scan_roop] < 20)m_RangeData.ranges[scan_roop]        = 0;
			scan_data[scan_roop] = m_RangeData.ranges[scan_roop];
		}

		//##########################
		//### �����W�f�[�^�̏��� ###
		//##########################
		//�����W�f�[�^���I�u�W�F�N�g�ɕ����Ă��̊J�n�_�ƏI���_��ݒ�
		int stid = 44;//240;
		int edid = 724;//528;
		int mdid = 384;
		for(int pt = 0; pt < 100; pt++)start[pt] = 0;
		int obj = 0;
		bool find_flag = 0;	//### 20160725 K.Suzuki : �^�[�Q�b�g�����o�ł��Ă��邩�������t���O
		double mini_x = 9999;
		double mini_y = 9999;
		//��Q���Ƃ̋���
		double dist_avoid_object;
		double tmp_dist_avoid_object = 9999;
		int shape_pattern;
		double start_obj_point[2] = {0};
		double end_obj_point[2]   = {0};

		for(int i = stid; i <= edid; i++)	//### �X�L�����͈͂𐧌��ii-1�ɃA�N�Z�X���邽�߁Ai�͕K��1�ȏォ��J�n�j ###
		{
			urg_ang = (mdid-i)*2*PI/1024;
			urg_x[i-stid] = m_RangeData.ranges[i] * sin(urg_ang);
			urg_y[i-stid] = m_RangeData.ranges[i] * cos(urg_ang);

			if(abs(scan_data[i] - scan_data[i-1]) > 200)
			{
				if(start[obj] != 0)
				{
					//### ���łɊJ�n�_���������Ă���Ƃ��I���_��ݒ肵�A���������߂�
					//### Object detection.
					// double ang = 0;
					// int dis = 0;
					// end[obj] = i-1;
					double start_ang = 0;
					double end_ang   = 0;
					double start_dis = 0;
					double end_dis   = 0;
					
					end[obj] = i - 1;
					
					start_ang = (start[obj] - mdid)*2*PI/1024;				//start[]�Ń��W�A���P�ʂ̊p�x�Z�o
					end_ang   = (end[obj]   - mdid)*2*PI/1024;				//end[]�Ń��W�A���P�ʂ̊p�x�Z�o
					start_dis = scan_data[start[obj]];					    //start[]�Ńf�[�^���ЂƂ܂������Ƃ��ĕۑ�
					end_dis   = scan_data[end[obj]];						//end[]�f�[�^���ЂƂ܂������Ƃ��ĕۑ�

					start_obj_point[0] = -1 * start_dis * sin(start_ang);	//double�^��X���W���v�Z
					start_obj_point[1] =      start_dis * cos(start_ang);	//double�^��Y���W���v�Z
					end_obj_point[0]   = -1 * end_dis   * sin(end_ang);		//double�^��X���W���v�Z
					end_obj_point[1]   =      end_dis   * cos(end_ang);		//double�^��Y���W���v�Z

					double deff_x = 0, deff_y = 0;
					deff_x = start_obj_point[0] - end_obj_point[0];
					deff_y = start_obj_point[1] - end_obj_point[1];
					//object�̒��S���W
					object_point[obj][0] = (start_obj_point[0] + end_obj_point[0]) / 2;//double�^��X���W���v�Z
					object_point[obj][1] = (start_obj_point[1] + end_obj_point[1]) / 2;//double�^��Y���W���v�Z
					//### ���{�b�g����̋�����human_dist(�����l=����9999)���߂��I�u�W�F�N�g��
					//### Closest object within the search area is selected.
					if(human_dist > sqrt((object_point[obj][0]-50)*(object_point[obj][0]-50) + (object_point[obj][1]-50)*(object_point[obj][1]-50)))//�Z���T�̂����-100�ŕ␳����
					{
						if((abs(lock_point[0]-(object_point[obj][0]-50))<300)&&(abs(lock_point[1]-(object_point[obj][1]-50))<300))
						{
							human_urg[0] = object_point[obj][0];
							human_urg[1] = object_point[obj][1] - 50;
						}
						//���{�b�g�̑O���ɂ����Q���̌`�󔻒f
						else if((abs(object_point[obj][0]) < 1000) && (object_point[obj][1] < 1500) && (0 < object_point[obj][1]))
						{
							if(start_obj_point[0] != 0 && end_obj_point[0] != 0)
							{
								shape_pattern = shape_discrimination(stid, edid, mdid, obj, start_obj_point[0], start_obj_point[1], end_obj_point[0], end_obj_point[1]);
							}
							if(shape_pattern == 1)      std::cout<<"##############LINE#############"<<std::endl;
							else if(shape_pattern == 2) std::cout<<"############Othres:2###########"<<std::endl;
							else if(shape_pattern == 3) std::cout<<"############Others:3###########"<<std::endl;
						}
					}
					obj++;
				}
				if(start[obj] == 0) start[obj] = i;	//### ���ݒ�or�ď������Ȃ�A�J�n�_��ݒ�
			}
			//shape_pattern �����Ƃɂ�����Q�����
			// Line -> �[�_�Aother -> �ł��߂��_
			// 1:Line, 2:other
			if(shape_pattern == 1)
			{
				if(abs(start_obj_point[0]) <= abs(end_obj_point[0]))
				{
					mini_x = start_obj_point[0];
					mini_y = start_obj_point[1];
				}
				else
				{
					mini_x = end_obj_point[0];
					mini_y = end_obj_point[1];
				}
			}
			else
			{
				if(-300 <= urg_x[i-stid] && urg_x[i-stid] <= 300)
				{
					if(20 <= urg_y[i-stid] && urg_y[i-stid] <= 500)
					{
						dist_avoid_object = sqrt(urg_x[i-stid] * urg_x[i-stid] + urg_y[i-stid] * urg_y[i-stid]);
						//��O�̏�Q�����߂������ꍇmini_x,mini_y���X�V����
						if(tmp_dist_avoid_object >= dist_avoid_object)
						{
							mini_x = urg_x[i-stid];
							mini_y = urg_y[i-stid];
						}
						tmp_dist_avoid_object = dist_avoid_object;
					}
					else if(urg_y[i-stid] > 20 && urg_y[i-stid] < 1000) slow_flag = 1;
				}
			}
			// urg�X�L�����̃��[�v�I��
		}
		if(obj == 0) obst_flag = 0;
		
		std::cout<<"mini_x= "<<mini_x<<std::endl;
		std::cout<<"mini_y= "<<mini_y<<std::endl;

		//mini_x,y�͈̔͂�flag
		//obst_flag -> avoid_flag
		if      ((mini_x > -200 && mini_x <= 200) && (mini_y > 30  && mini_y <= 250)) obst_flag  = 1;
		else if ((mini_x > -300 && mini_x <= 300) && (mini_y > 250 && mini_y <= 500)) avoid_flag = 1;
		else avoid_flag = obst_flag = 0;

		//����t���O�����Ă��� or �l��������x�O�ɂ����畜�A�t���O�܂�
		if(avoid_flag || obst_flag) recovery_flag = 0;
		if(abs(human_point[0]) < 100)    recovery_flag = 0;
		if(recovery_flag || !avoid_flag) mini_x = mini_y = 9999;
		//�|�e���V�����@�̌v�Z
		//mini_x,y���Z���T�ʒu���l�����ďC��
		mini_x -=50;
		mini_y -=50;
		if(avoid_flag || recovery_flag)
		{
			double potential_v[2] = {0};
			potential_route(0, 0, human_point[0], human_point[1], mini_x, mini_y, potential_v);
			potential_vx = potential_v[0];
			potential_va = potential_v[1];
			//va����/2�����]������p�x�ɂ���A�E�ɉ������ꍇ�͕�
			potential_va = potential_va - PI/2;		
		}

		//���̂��߂�����ꍇ�A��~
		if(obst_flag){
			Beep(300, 100);
			just_Move(0, 0, 0);
		}

//### 20161210 K.Suzuki ###
#ifdef NEW_TRACKING
		//#############################
		//### Target TORSO position ###
		//#############################
		if(user_infos.target >= 0){
			printf("\n\n\n<<< TARGET USER ID = %d >>>\n\n\n\n", user_infos.target);
			sUSER_INFO *pui = user_infos.user_info;;
			for(int i=0; i<user_infos.num; i++, pui++){
				if(pui->id != user_infos.target) continue;

				g_kinect_data_x = pui->torso_x;
				g_kinect_data_z = pui->torso_z;
				break;
			}
		}
		if(((g_kinect_data_x != last_kinect_x) && (g_kinect_data_z != last_kinect_z)))
		{
			//### �����W�f�[�^�őI�ʂ��ꂽ�ΏۃI�u�W�F�N�g�ʒuhuman_point����XY������300�����Ƀg���\�f�[�^������Ƃ�rock=200
			//### �˃g���\�f�[�^�͋���臒lrock�𐧌䂷�邱�Ƃ����Ɏg���Ă���
			if(abs(-g_kinect_data_x - human_point[0])<300 && abs(g_kinect_data_z - (human_point[1]/*+300*/))<300)
			{
				rock = 200;
			}
			else
			{
				rock = 500;
			}
			//### 20160801 K.Suzuki : �g���\�ʒu���^�[�Q�b�g�Ƃ���
			human_point[0] = -g_kinect_data_x;
			human_point[1] =  g_kinect_data_z;
			human_dist     = sqrt(human_point[0]*human_point[0]+human_point[1]*human_point[1]);
			lock_point[0]  = human_point[0];
			lock_point[1]  = human_point[1]; 
			find_flag      = 1;
			//�l��������ꂽ�畜�A��̉���t���O�܂�
			recovery_flag = 0;
			avoid = 0;
		}
		else if(abs(human_urg[0]) > 0 && human_urg[1] > 0)
		{
			human_point[0] = human_urg[0];
			human_point[1] = human_urg[1];
			human_dist     = sqrt(human_point[0]*human_point[0] + human_point[1]*human_point[1]);
			find_flag      = 1;
		}
		else find_flag = 0;

		
		//�X�V�̊m�F(�l�̌������̊m�F)
		if (last_human_point[0] == human_point[0] && last_human_point[1] == human_point[1] && !stay_flag)
		{
			no_update_count += 1;
			human_point[0] = human_urg[0];
			human_point[1] = human_urg[1];
			human_dist     = sqrt(human_point[0]*human_point[0] + human_point[1]*human_point[1]);
			find_flag      = 1;

			if(no_update_count >= 5)
			{
				no_update_count = 0;
				find_flag       = 0;
				//stay_flag       = 1;
				just_Move(0, 0, 0);
				//Beep(4000, 500);
			}
		}
		else no_update_count = 0;

		if(find_flag == 0 && stay_flag!=1){
			std::cout<<"#####################Prediction mode#################"<<std::endl;
			double dist_prediction, dist_x, dist_y;
			predict_count += 1;
			dist_x = prediction_human_point_x[predict_count-1] - human_point[0];
			dist_y = prediction_human_point_y[predict_count-1] - human_point[1];
			dist_prediction = sqrt(dist_x * dist_x + dist_y * dist_y);
			if(dist_prediction <= 500){
				std::cout<<"##########################################"<<std::endl;
				std::cout<<"###############predict point##############"<<std::endl;
				std::cout<<"##########################################"<<std::endl;
				human_point[0] = prediction_human_point_x[predict_count-1];
				human_point[1] = prediction_human_point_y[predict_count-1];
				Beep(4000, 500);
				//Beep(4000, 250);
				if(predict_count == 5) {
					stay_flag = 1;
					predict_count = 0;
				}
			}
			else stay_flag = 1;
		}
		else predict_count = 0;
		// human_point�̏����͂����܂�
		
		//�X�V�m�F�p
		last_kinect_x   = g_kinect_data_x;
		last_kinect_z   = g_kinect_data_z;
		
		last_human_point[0] = human_point[0];
		last_human_point[1] = human_point[1];
		
		//�\���p�f�[�^�o�� & �󂯎�菈��
		double robotpose_x, robotpose_y, robotheading;
		double human_position_odox, human_position_odoy;
		
		//�I�h���g�����烏�[���h���W�n�ɕϊ��A�o��
		if(m_RobotPose2DIn.isNew()){
			robotpose_x = -m_RobotPose2D.data.position.y;
			robotpose_y =  m_RobotPose2D.data.position.x;
			robotheading   =  m_RobotPose2D.data.heading;
			m_RobotPose2DIn.read();
			human_position_odox = (human_point[0]*cos(robotheading)-human_point[1]*sin(robotheading))/1000 + robotpose_x;
			human_position_odoy = (human_point[0]*sin(robotheading)+human_point[1]*cos(robotheading))/1000 + robotpose_y;
		}
		m_HumanPoint.data[0] = human_position_odox;
		m_HumanPoint.data[1] = human_position_odoy;
		if(find_flag)m_HumanPointOut.write();
		
		//�\���f�[�^�����[���h���W�n����ϊ��A�z��Ɋi�[
		if(m_PredictionHumanPointIn.isNew()){
			m_PredictionHumanPointIn.read();
			
			prediction_log_x = m_PredictionHumanPoint.data[0]; 
			prediction_log_y = m_PredictionHumanPoint.data[1];
			for(int i = 0;i < 5;i++){
				
				double prediction_human_point_odox, prediction_human_point_odoy;
				double tmp_x, tmp_y;
				
				prediction_human_point_odox   = m_PredictionHumanPoint.data[i*2];
				prediction_human_point_odoy   = m_PredictionHumanPoint.data[i*2+1];
					
				tmp_x = prediction_human_point_odox - robotpose_x;
				tmp_y = prediction_human_point_odoy - robotpose_y;
								
				prediction_human_point_x[i] = (tmp_x *  cos(robotheading) + tmp_y * sin(robotheading)) * 1000;
				prediction_human_point_y[i] = (tmp_x * -sin(robotheading) + tmp_y * cos(robotheading)) * 1000;
			}
		}
		//�\�������܂�

		std::cout<<"odo_x = "<<human_position_odox<<" odo_z = "<<human_position_odoy<<std::endl;
//### 20161210 K.Suzuki ###
#endif //NEW_TRACKING

		//���b�N�I������
		if(find_flag)	//### 20160805 K.Suzuki : Everything is OK?
		{
			//### �����W�f�[�^�őI�ʂ��ꂽ�ΏۃI�u�W�F�N�g�ʒuhuman_point��lock_point����XY������rock�����ɂ���Ƃ��A
			//### lock_point��human_point�Œu��������i�X�V�����j
			lock_point[0] = human_point[0];
			lock_point[1] = human_point[1]; 
			if(fplog != NULL) fprintf(fplog, "lock_on\n");	//### 20160725 K.Suzuki : log
		}
		
		time_t timer;
		struct tm *t_st;
		time(&timer);
		t_st = localtime(&timer);
		fprintf(fp,"%d:%d:%d,",t_st->tm_hour,t_st->tm_min,t_st->tm_sec);
		fprintf(fp,"%lf,%lf,", human_point[0],      human_point[1]);
		fprintf(fp,"%lf,%lf,", human_position_odox, human_position_odoy);
		fprintf(fp,"%lf,%lf,", prediction_log_x, prediction_log_y);
		fprintf(fp,"%lf,%lf,", mini_x,              mini_y);
		if(obst_flag)  fprintf(fp,"obst,");
		else if(avoid_flag)
		{
			if(mini_x > 0)         fprintf(fp,"Right_avoid,");
			else                   fprintf(fp,"Left_avoid,");
			if(shape_pattern == 1) fprintf(fp,"Line,");
			else                   fprintf(fp,"Others,");
		}
		else if(recovery_flag)     fprintf(fp,"Recovery,,");
		else                       fprintf(fp,"onTracking,,");
		if((avoid_flag || recovery_flag)) fprintf(fp,"%lf,%lf,", potential_vx, potential_va);
		else fprintf(fp,",,");

		//############################
		//### �Ǐ]�ړ��R�}���h���� ###
		//### Motion.              ###
		//############################
#if 0
#else
		//### 20160910 OK ###
		mini_dist    = human_dist;
		mini_heading = atan2(human_point[0],human_point[1])/2/PI*360;
		if(mini_heading>180) mini_heading = mini_heading - 360;
		//mini_heading=-mini_heading;
		
		find_character = 1;
		if (find_flag == 1 /*&& find_character==1*/)
		{
			// if(stay_flag||obst_flag) goto END;	//### 20160725 K.Suzuki : No motion
			//##############################
			//## �|�e���V�����@�ɂ���� ##
			//##############################
			if(avoid_flag || recovery_flag)
			{
				Beep(4000, 100);
				if(potential_vx > 0.1)         vx = 0.1;
				else if(potential_vx <= -0.05) vx = -0.05;
				else vx = potential_vx;
				va = potential_va;
				if(avoid_flag)    Beep(4000,100);
				if(recovery_flag) Beep(100,100);
				
				if(!stay_flag) just_Move(vx, 0, va);
				std::cout<<"va,vx="<<va<<","<<vx<<std::endl;
				//���A�t���O���Ă�
				recovery_flag = 0;
				avoid += 1;
			}
			else
			{	
				if(mini_heading > 10)	//turn right (over 10 degrees)
				{
					for(int time = 0; time < 5; time ++){
						urg_read();
						if(center_counter >= 10){
							time_Move(550, m_black_ship.data.vx, 0.0, 0.1);//2017_01_25 Shimoyama 750->550
							time_Move(250, 0.0, 0.0, 0.0);		
						}else if(right_counter >= 10){
							time_Move(550, 0.2, 0.0, 0.0);// 2017_01_25 Shimoyama 750->550 0.2->0.05 2017_02_09 Shimoyama 750
							time_Move(250, 0.0, 0.0, 0.0);
						}else if(time == 4){
							return RTC::RTC_OK;
						}else{
							break;
						}
					}
					std::cout<<"����="<<va<<std::endl;
				}
				else if(mini_heading<-10) //turn left (over 10 degrees)
				{
					for(int time = 0; time < 5; time ++){
						urg_read();
						if(center_counter >= 10){
							time_Move(550, m_black_ship.data.vx, 0.0, -0.1); // 2017_01_25 Shimoyama 750->550  
							time_Move(250, 0.0, 0.0, 0.0);
						}else if(left_counter >= 10){
							time_Move(550, 0.2 , 0.0, 0.0);// 2017_01_25 Shimoyama 750->550 2017_02_09 Shimoyama 750
							time_Move(250, 0.0, 0.0, 0.0);
						}else if(time == 4){
							return RTC::RTC_OK;
						}else{
							break;
						}
					}
					std::cout<<"����="<< va <<std::endl;
				}
				//### Backword is permitted even when obst_flag is true.
				if(mini_dist < 800) //move backwards
				{
					vx = max(0.15, (800 -(double)mini_dist) / MAX_RANGE) * -2 / 3;
					va = -mini_heading * m_va_value * PI/180/25;

					if(!stay_flag) just_Move(vx, 0, va);
					std::cout<<"���="<<-vx<<std::endl;
				}
				else if(mini_dist>1200) //move forwards and rotate
				{
					if(slow_flag) vx_value=0.5;
					else vx_value=0.7;
					slow_flag = 0;
					vx = max(0.15, ((double)mini_dist - 1200) / MAX_RANGE)*vx_value; // 2017_01_25 Shimoyama �������邽�߁@vx/3*2;
					va = -mini_heading * m_va_value*PI/180/25;	//2017_01_24 Shimoyama concierge�l���ɑ΂��ċt��]���Ă������߃}�C�i�X�ɁD���񂪑������Đl���������₷������10���̈�ɁD;
					
					if(!stay_flag) just_Move(vx, 0, va);
					std::cout<<"�O�i="<<vx<<" "<<va<<std::endl;
				}
				else //just rotate
				{
					va = -mini_heading * m_va_value*PI/180/25;//2017_01_24 Shimoyama concierge�l���ɑ΂��ċt��]���Ă������߃}�C�i�X�ɁD���񂪑������Đl���������₷������10���̈�ɁD;
					if(!stay_flag) just_Move(0, 0, va);
				}
			}
			fprintf(fp,"%lf,%lf,", m_black_ship.data.vx, m_black_ship.data.va);
			fprintf(fp,"\r");
		}
		else if(!find_flag){
			
			//human_point[0] = lock_point[0];
			//human_point[1] = lock_point[1];
			//Beep(1000, 100);	//### 20160725 K.Suzuki : beep
			//### �����I�ɒ�~
			just_Move(0, 0, 0);
			if(fplog != NULL) fprintf(fplog, "\n");	//### 20160725 K.Suzuki : log
			goto END;	//### 20160725 K.Suzuki : No motion.
		}
#endif
	}
	END:;

    BOOL ret = GetMessage( &msg, NULL, 0, 0 );  // ���b�Z�[�W���擾����
	if( ret == 0 || ret == -1 )
	{
		// �A�v���P�[�V�������I�������郁�b�Z�[�W�����Ă�����A
		// ���邢�� GetMessage() �����s������( -1 ���Ԃ��ꂽ�� �j�A���[�v�𔲂���
	}
	else
	{
		// ���b�Z�[�W����������
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	UpdateWindow( hWnd );
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t object_tacking_concierge::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t object_tacking_concierge::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t object_tacking_concierge::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t object_tacking_concierge::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t object_tacking_concierge::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


bool object_tacking_concierge::time_Move(int time,double x,double y,double a){	
	m_black_ship.data.vx=x;
	m_black_ship.data.vy=y;
	//m_black_ship.data.va=a
	m_black_ship.data.va=-a/27;	//2017_01_24 Shimoyama concierge�l���ɑ΂��ċt��]���Ă������߃}�C�i�X�ɁD���񂪑������Đl���������₷������10���̈�ɁD
	if(!stay_flag && !obst_flag) m_black_shipOut.write();	//### 
	Sleep(time);
	return true;
}

bool object_tacking_concierge::just_Move(double x,double y,double a){
	m_black_ship.data.vx=x;
	m_black_ship.data.vy=y;
	m_black_ship.data.va=a;
	m_black_shipOut.write();
	return true;
}
void object_tacking_concierge::wait(int time)//##time/1000�ҋ@
{
	clock_t finish_wait=0;
	clock_t start_wait=0;

	start_wait = clock();

	while(1){
		finish_wait = clock();
		if((finish_wait-start_wait)>time) break;
	}
}

void object_tacking_concierge::urg_read(){
	if(!m_RangeDataIn.isEmpty()){
		m_RangeDataIn.read();
		right_counter=0; center_counter=0; left_counter=0;
		for(int scan_roop=44;scan_roop<=724;scan_roop++){
			if(m_RangeData.ranges[scan_roop]>MAX_RANGE) m_RangeData.ranges[scan_roop]=0;
			if(m_RangeData.ranges[scan_roop]<20)        m_RangeData.ranges[scan_roop]=0;
			urg_ang = (384- scan_roop)*2*PI/1024;
			urg_x[scan_roop-44] = m_RangeData.ranges[scan_roop] * sin(urg_ang);
			urg_y[scan_roop-44] = m_RangeData.ranges[scan_roop] * cos(urg_ang);
		
			//if there is obstacle for right
			if((urg_x[scan_roop-44] > 250 && urg_x[scan_roop-44] <= 350) && (urg_y[scan_roop-44] > -350 && urg_y[scan_roop-44] < -250)){
				right_counter++;
			//if there is obstacle for center
			}else if((urg_x[scan_roop-44] > -250 && urg_x[scan_roop-44] <= 250) && (urg_y[scan_roop-44] > 30 && urg_y[scan_roop-44] < 100)){
				center_counter++;
			//if there is obstacle for left
			}else if((urg_x[scan_roop-44] > -350 && urg_x[scan_roop-44] <= -250) && (urg_y[scan_roop-44] > -350 && urg_y[scan_roop-44] < -250)){
				left_counter++;
			}
		}
	}
}

extern "C"
{
 
  void object_tacking_conciergeInit(RTC::Manager* manager)
  {
    coil::Properties profile(object_tacking_concierge_spec);
    manager->registerFactory(profile,
                             RTC::Create<object_tacking_concierge>,
                             RTC::Delete<object_tacking_concierge>);
  }
  
};


// �E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch( msg )
	{
	case WM_DESTROY:  // �E�B���h�E��j������Ƃ�
		PostQuitMessage( 0 );
		return 0;
	case WM_PAINT:
		draw_window();
		return 0;
	}

	// ���̃��b�Z�[�W�́A�f�t�H���g�̏������s��
	return DefWindowProc( hWnd, msg, wp, lp );
}

// �J�n�ʒu
int WINAPI window_create(HINSTANCE hInst)
{
	// �E�B���h�E�N���X�̏���ݒ�
	wc.cbSize = sizeof(wc);               // �\���̃T�C�Y
	wc.style = CS_HREDRAW | CS_VREDRAW;   // �X�^�C��
	wc.lpfnWndProc = WndProc;             // �E�B���h�E�v���V�[�W��
	wc.cbClsExtra = 0;                    // �g�����P
	wc.cbWndExtra = 0;                    // �g�����Q
	wc.hInstance = hInst;                 // �C���X�^���X�n���h��
	wc.hIcon = (HICON)LoadImage(          // �A�C�R��
		NULL, MAKEINTRESOURCE(IDI_APPLICATION), IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE | LR_SHARED
	);
	wc.hIconSm = wc.hIcon;                // �q�A�C�R��
	wc.hCursor = (HCURSOR)LoadImage(      // �}�E�X�J�[�\��
		NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
		0, 0, LR_DEFAULTSIZE | LR_SHARED
	);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // �E�B���h�E�w�i
	wc.lpszMenuName = NULL;                     // ���j���[��
	wc.lpszClassName = _T("Default Class Name");// �E�B���h�E�N���X��
	
	// �E�B���h�E�N���X��o�^����
	if( RegisterClassEx( &wc ) == 0 ){ return 1; }

	// �E�B���h�E���쐬����
	hWnd = CreateWindow(
		wc.lpszClassName,      // �E�B���h�E�N���X��
		_T("URG_Data"),  // �^�C�g���o�[�ɕ\�����镶����
		WS_OVERLAPPEDWINDOW,   // �E�B���h�E�̎��
		CW_USEDEFAULT,         // �E�B���h�E��\������ʒu�iX���W�j
		CW_USEDEFAULT,         // �E�B���h�E��\������ʒu�iY���W�j
		500,         // �E�B���h�E�̕�
		500,         // �E�B���h�E�̍���
		NULL,                  // �e�E�B���h�E�̃E�B���h�E�n���h��
		NULL,                  // ���j���[�n���h��
		hInst,                 // �C���X�^���X�n���h��
		NULL                   // ���̑��̍쐬�f�[�^
	);
	if( hWnd == NULL ){ return 1; }

	// �E�B���h�E��\������
	ShowWindow( hWnd, SW_SHOW );
	UpdateWindow( hWnd );
	return 0;
}

int draw_window()
{
	hdc=GetDC(hWnd);
	HPEN hPen,hPen2,hPen3,hPen4,hPen5,hPen6;	//### 20160725 K.Suzuki
	static HBITMAP  hMemBmp,hOldMemBmp;
	double theta,dist,x_point,y_point, scale_control=10;
	int int_x,int_y;

	// �������f�o�C�X�R���e�L�X�g���쐬����
	hMemBmp = CreateCompatibleBitmap( hdc, 500, 500 );
	memDC = CreateCompatibleDC( hdc );
	hOldMemBmp = (HBITMAP)SelectObject( memDC, hMemBmp );

	hPen = CreatePen( PS_SOLID, 2, RGB(255,255,255) ); // ���̘_���y�����쐬����
	hPen2 = CreatePen( PS_SOLID, 1, RGB(100,100,100) ); // �D�F�̘_���y�����쐬����
	hPen3 = CreatePen( PS_SOLID, 1, RGB(0,0,255) ); // �j���̘_���y�����쐬����
	hPen4 = CreatePen( PS_SOLID, 5, RGB(200,10,10) ); // �Ԃ̘_���y�����쐬����
	hPen5 = CreatePen( PS_SOLID, 5, RGB(10,10,200) ); // �̘_���y�����쐬����
	hPen6 = CreatePen( PS_SOLID, 5, RGB(255,255,0) ); // ���F�̘_���y�����쐬����	//### 20160725 K.Suzuki

	int xo = 250;
	int yo = 300;

	//### human_point�𒆐S��rock�͈͂��D�F�̉~�ŕ`�� ###
	SelectObject( memDC, hPen2 );
	MoveToEx(memDC,xo,yo,NULL);
	Ellipse( memDC,
	(int)(human_point[0]/scale_control-rock/scale_control+xo),
	(int)(-human_point[1]/scale_control-rock/scale_control+yo),
	(int)(human_point[0]/scale_control+rock/scale_control+xo),
	(int)(-human_point[1]/scale_control+rock/scale_control+yo));  // �ȉ~��`�悷��

	//### �g���D�F�ŕ`�� ###
	MoveToEx(memDC ,250 ,0 , NULL);
	LineTo(memDC , 250 , 500);
	MoveToEx(memDC ,0 ,300 , NULL);
	LineTo(memDC , 500 , 300);

	//### �r�����D�F�ŕ`�� ###
	SelectObject( memDC, hPen2 );

	for(int count=0;count<=10;count++)
	{
		MoveToEx(memDC ,count*50 ,0 , NULL);
		LineTo(memDC ,count*50 ,500);
		MoveToEx(memDC ,0 ,count*50 , NULL);
		LineTo(memDC ,500 ,count*50);
	}

	//### �����W�f�[�^��ŕ`�� ###
	SelectObject( memDC, hPen3 );

	for(int count=44 ; count<=725 ; count++){
		MoveToEx(memDC,xo,yo,NULL);
		theta=(count-384)*2*PI/1024;				//���W�A���P�ʂ̊p�x�Z�o
		dist=scan_data[count];					//�f�[�^���ЂƂ܂������Ƃ��ĕۑ�
		x_point=-1*dist*sin(theta)/(scale_control);	//double�^��X���W���v�Z
		y_point=dist*cos(theta)/(scale_control);		//double�^��Y���W���v�Z

		int_x =(int) x_point+xo;								//int�^��X���W���v�Z�����S�ɍ��킹��
        int_y =(int) -y_point+yo;								//int�^��Y���W���v�Z�����S�ɍ��킹��
    	if(scan_data[count] != 0){					//�f�[�^��0�̂Ƃ��͖����A0�ȊO�̏ꍇ�͕`��
		LineTo(memDC,int_x,int_y);
		}
	}
	
	//### ���_����human_point�܂ł�Ԃ̃��C���ŕ`�� ###
	SelectObject( memDC, hPen4 );
	MoveToEx(memDC,xo,yo,NULL);
	x_point=human_point[0]/(scale_control);	//double�^��X���W���v�Z
	y_point=human_point[1]/(scale_control);		//double�^��Y���W���v�Z

	int_x =(int) x_point+xo;								//int�^��X���W���v�Z�����S�ɍ��킹��
    int_y =(int) -y_point+yo;								//int�^��Y���W���v�Z�����S�ɍ��킹��
	LineTo(memDC,int_x,int_y);

	//### ���̗̈��`�� ###
	for(int roop=0;start[roop]!=0;roop++){
		//### ���̗̈�̎n�I�[��Ԃ̃��C���ŕ`�� ###
		theta=(start[roop]-384)*2*PI/1024;				//���W�A���P�ʂ̊p�x�Z�o
	    dist=scan_data[start[roop]];					//�f�[�^���ЂƂ܂������Ƃ��ĕۑ�
		x_point=-1*dist*sin(theta)/(scale_control);	//double�^��X���W���v�Z
		y_point=dist*cos(theta)/(scale_control);		//double�^��Y���W���v�Z
		int_x =(int) x_point+xo;				//int�^��X���W���v�Z�����S�ɍ��킹��
		int_y =(int) -y_point+yo;								//int�^��Y���W���v�Z�����S�ɍ��킹��
		MoveToEx(memDC,int_x,int_y,NULL);
		theta=(end[roop]-384)*2*PI/1024;				//���W�A���P�ʂ̊p�x�Z�o
	  dist=scan_data[end[roop]];					//�f�[�^���ЂƂ܂������Ƃ��ĕۑ�
		x_point=-1*dist*sin(theta)/(scale_control);	//double�^��X���W���v�Z
		y_point=dist*cos(theta)/(scale_control);		//double�^��Y���W���v�Z
		int_x =(int) x_point+xo;				//int�^��X���W���v�Z�����S�ɍ��킹��
		int_y =(int) -y_point+yo;								//int�^��Y���W���v�Z�����S�ɍ��킹��
	    LineTo(memDC,int_x,int_y);

		//### object_point�𔼌a10�̐Ԃ̉~�ŕ`�� ###
		
			MoveToEx(memDC,xo,yo,NULL);
			Ellipse( memDC,
			(int)(object_point[roop][0]/scale_control-10+xo),
			(int)(-object_point[roop][1]/scale_control-10+yo),
			(int)(object_point[roop][0]/scale_control+10+xo),
			(int)(-object_point[roop][1]/scale_control+10+yo));  // �ȉ~��`�悷��
	}

	//### �g���\�ʒu��̉~�ŕ`�� ###
	SelectObject( memDC, hPen5 );
	//�L�l�N�g���犄��o�����ʒu��\������
 	if(-g_kinect_data_x==last_kinect_x && -g_kinect_data_z==last_kinect_z){
	Ellipse(memDC,NULL,NULL,NULL,NULL);	
	}
	else{
	Ellipse( memDC,
	(int)(-g_kinect_data_x/scale_control-50/scale_control+xo),
	(int)((-g_kinect_data_z)/scale_control-50/scale_control+yo),
	(int)(-g_kinect_data_x/scale_control+50/scale_control+xo),
	(int)((-g_kinect_data_z)/scale_control+50/scale_control+yo));  // �ȉ~��`�悷��
	}
	//### 20160725 : Added.
	//### lock_point�𒆐S��rock�͈͂����F�̉~�ŕ`�� ###
	if(stay_flag) SelectObject( memDC, hPen3 );
	else          SelectObject( memDC, hPen6 );
	MoveToEx(memDC,xo,yo,NULL);
	Ellipse( memDC,
	(int)(lock_point[0]/scale_control-rock/scale_control+xo),
	(int)(-lock_point[1]/scale_control-rock/scale_control+yo),
	(int)(lock_point[0]/scale_control+rock/scale_control+xo),
	(int)(-lock_point[1]/scale_control+rock/scale_control+yo));  // �ȉ~��`�悷��

	SelectObject( memDC, hPen );

	BitBlt(hdc , 0 , 0 , 500 , 500 , memDC , 0 , 0 , SRCCOPY);

	//DeleteObject( hBlocksBmp );  // ���[�h�����r�b�g�}�b�v���폜����
	DeleteObject( hMemBmp );
	DeleteObject( hOldMemBmp );
	DeleteObject( hPen );
	DeleteObject( hPen2 );
	DeleteObject( hPen3 );
	DeleteObject( hPen4 );
	DeleteObject( hPen5 );
	ReleaseDC(hWnd , hdc);
	DeleteDC(memDC);
	return 0;
}

bool object_tacking_concierge::log_open()
{
	//if(open_check == false){
		//���ԏ�񂩂烆�j�[�N�ȃt�@�C�����𐶐�
		time_t timer;
		struct tm *t_st;
		time(&timer);
		t_st = localtime(&timer);
		char filename[50];

		sprintf(filename,"log\\%04d_%02d%02d_%02d%02d%02d_�|�e���V�����@.csv" ,t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec);
	
		//�l�̈ʒu���O�t�@�C���̃I�[�v��
		if ((fp = fopen(filename, "w")) == NULL) {
			printf("file open error!!\n");
			return false;
		}
		fprintf(fp,"Time, human_X, human_Y, human_position_odox, human_position_odoz, pre_human_position_x, pre_human_position_odoz, object_x, object_y, status, object, potential_vx, potential_va, vx, va");
		fprintf(fp,"\r\n");

		open_check = true;
		return 0;
	//}
}
//######�|�e���V�����@#########
double object_tacking_concierge::potential_field(double x, double y, double obst_x, double obst_y, double goal_x, double goal_y)
{
	double obst_potential, goal_potential, potential_all;
	double weight_goal   =  10;
	double weight_obst   =   1;
	double potential_max = 1.0;
	double potential_min = -1.0;

	//��Q���̍��W�̃|�e���V������max
	if((obst_x == x) && (obst_y == y)) obst_potential = potential_max;
	else obst_potential  = 1 / sqrt((x - obst_x) * (x - obst_x) + (y - obst_y) * (y - obst_y));
	obst_potential *= weight_obst;
	
	if(obst_potential > potential_max) obst_potential = potential_max;

	//�S�[���̍��W�̃|�e���V������min
	if (goal_x == x && goal_y == y)	goal_potential = potential_min;
	else goal_potential  = - 1 / sqrt((x - goal_x) * (x - goal_x) + (y - goal_y) * (y - goal_y));
	goal_potential *= weight_goal;
	
	if(goal_potential < potential_min) goal_potential = potential_min;
	potential_all = obst_potential + goal_potential;
	
	return potential_all;
}

void object_tacking_concierge::potential_route(double x, double y, double goal_x, double goal_y, double obst_x, double obst_y, double potential_result[2])
{
	double delt  = 0.01;
	double speed = 0.1;
	double dx, dy, xy, vx, vy, v, va;
	//dx,dy�����߂ĕΔ���
	dx = potential_field(x + delt, y        , obst_x, obst_y, goal_x, goal_y);
	dy = potential_field(x       , y + delt , obst_x, obst_y, goal_x, goal_y);
	xy = potential_field(x       , y        , obst_x, obst_y, goal_x, goal_y);
	vx = -(dx - xy) / delt;
	vy = -(dy - xy) / delt;

	v  = sqrt(vx * vx + vy * vy);

	vx /= v / speed;
	vy /= v / speed;

	va = atan2(abs(vy), vx);
	potential_result[0] = vy;
	potential_result[1] = va;
	//return vy;
}
//##########�����܂�###############

//�`�󔻒f
int object_tacking_concierge::shape_discrimination(int stid, int edid, int mdid, int obj, double start_obj_point_x, double start_obj_point_y, double end_obj_point_x, double end_obj_point_y)
{
	// urg_ang, urg_x, urg_y, object_point[0], obj_point[1] : global
	// start_point ���� end_point �܂ł�urg_data��ǂݎ��
	// �������ۂ� -> �X������肩�ۂ�
	double object_trend, target_trend;
	double urg_x, urg_y, urg_next_x, urg_next_y, urg_next_ang;
	int next_i, match_count = 0;

	object_trend = atan2(abs(start_obj_point_x - end_obj_point_x) , abs(start_obj_point_y - end_obj_point_y));
	
	m_RangeDataIn.read();
	for(int i = stid; i <= edid; i++)
	{
		next_i = 5;

		urg_ang = (mdid-i) * 2 * PI / 1024;
		urg_next_ang = (mdid-i-next_i) * 2 * PI / 1024;
		urg_x = m_RangeData.ranges[i] * sin(urg_ang);
		urg_y = m_RangeData.ranges[i] * cos(urg_ang);
		urg_next_x = m_RangeData.ranges[i+next_i] * sin(urg_next_ang);
		urg_next_y = m_RangeData.ranges[i+next_i] * cos(urg_next_ang);

		if (urg_y > 0)
		{
			if (start_obj_point_x > urg_x)
			{
				if(end_obj_point_x > urg_x) break;
				target_trend = atan2(abs(urg_next_x - urg_x), abs(urg_next_y - urg_y));
				if(abs((target_trend - object_trend)) < 0.01) match_count += 1;
				if(match_count >= 2) break;
				//fprintf(fp,"%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,", obj, i, urg_x, urg_y, start_obj_point_x, start_obj_point_y, end_obj_point_x, end_obj_point_y, object_trend, target_trend, match_count); 				//fprintf(fp,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,", urg_x, urg_y, target_length, start_obj_point_x, start_obj_point_y, end_obj_point_x, end_obj_point_y, object_length, diff_length);
				//fprintf(fp,"\r");
			}
		}
		if(match_count > 10) break;
	}
	if(match_count >= 2) return 1;
	else return 2;
}

bool object_tacking_concierge::log_close()
{
	//if(open_check == true){
		  fclose(fp);	/* (5)�t�@�C���̃N���[�Y */
		  open_check = false;
	//}
  return true;
}