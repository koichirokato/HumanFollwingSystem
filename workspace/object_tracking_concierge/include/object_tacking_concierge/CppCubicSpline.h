/**
 *  @brief Cubic Spline header library
 *
 *  @author Atsushi Sakai
 *
 **/

#include <math.h>

using namespace std;

/**
 *  @brief Cubic Spline header library
 *
 *  @usage 
 *    vector<double> sy{0,object[1],human_point[1]};
 *    CppCubicSpline cppCubicSpline(sy);
 *    vector<double> rx;
 *    vector<double> ry;
 *    for(double i=1;i<=100;i++){
 *       rx.push_back(i);
 *       ry.push_back(cppCubicSpline.Calc(i));
 *    }
 */
class CppCubicSpline{
  public:
    CppCubicSpline(const double (&sy)[3]){

      //InitParameter(&y);

    }

    double Calc(double i){
        int j=int(floor(i));
        if(j<=0){
            j=0;
        }
        else if(j>0/*a_.size()*/){
            //j=(a_.size()/10-1);
              j=2;
		}

        double dt=i-j;
        double result=a_[j]+(b_[j]+(c_[j]+d_[j]*dt)*dt)*dt;
        return result;
    }

  private:
	double a_[30];
    double b_[30];
    double c_[30];
    double d_[30];
    double w_[30];

    void /*InitParameter*/cppCubicSpline(const double (&sy)[30]){
      //int ndata=y.size()-1;
        int ndata = 2;

      for(int i=0;i<=ndata;i++){
		  //a_.push_back(y[i])
		  a_[i]=sy[i];
		  std::cout<<"a_[i] = "<<a_[i]<<std::endl;
      }

      for(int i=0;i<ndata;i++){
        if(i==0){
          //c_.push_back(0.0);
			c_[i]=0.0;
        }
        else if(i==ndata){
          //c_.push_back(0.0);
            c_[i]=0.0;
		}
        else{
          //c_.push_back(3.0*(a_[i-1]-2.0*a_[i]+a_[i+1]));
		    c_[i]=3.0*(a_[i-1]-2.0*a_[i]+a_[i+1]);
		}
      }

      for(int i=0;i<=ndata;i++){
        if(i==0){
          //w_.push_back(0.0);
            w_[i]=0.0;
		}
        else{
          double tmp=3.0-w_[i-1];
          c_[i]=(c_[i]-c_[i-1])/tmp;
          //w_.push_back(1.0/tmp);
            w_[i]=1.0/tmp;    
		}
      }

      for(int i=(ndata-1);i>0;i--){
        c_[i]=c_[i]-c_[i+1]*w_[i];
      }

      for(int i=0;i<=ndata;i++){
        if(i==ndata){
          //d_.push_back(0.0);
          //b_.push_back(0.0);
            d_[i]=0.0;
			b_[i]=0.0;
		}
        else{
          //d_.push_back((c_[i+1]-c_[i])/3.0);
          //b_.push_back(a_[i+1]-a_[i]-c_[i]-d_[i]);
            d_[i]=(c_[i+1]-c_[i])/3.0;
			b_[i]=a_[i+1]-a_[i]-c_[i]-d_[i];
		}
      }

    }

};
