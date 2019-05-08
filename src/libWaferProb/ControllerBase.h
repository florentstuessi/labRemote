#ifndef __WaferProb_ControllerBase_H__
#define __WaferProb_ControllerBase_H__

/*
 * Define a general interface for a controller.
 * All length unit is in millimeter
 */
#include <string>

using namespace std;
class ControllerBase{

	protected:

		int status; // 0 is ok, non-zero indicates problems
		bool m_is_connected;

		// put it public so it's easy to access...
	public:
		float m_position[3];
		bool m_z_calibrated;

	public:
		ControllerBase();
		virtual ~ControllerBase();

		int get_status() { return status; }
		bool is_connected() { return m_is_connected; }

		// command lines are interpreted by this function.
		int run_cmd(const string& cmd);
		void print_cmd();

		virtual int connect() = 0;
		virtual int disconnect() = 0;

		virtual int write(const string& cmd) = 0; // send command to device

		// unit of length: millimeter
		virtual int set_speed(int axis, float value) = 0;

		// move w.r.t home-position
		virtual int mv_abs(int axis, float value) = 0;
		// move w.r.t current-position
		virtual int mv_rel(int axis, float value) = 0;

		virtual int stop() = 0; // stop motion immediately

		virtual int get_position() = 0;

		virtual int set_home() = 0;
		virtual int set_center() = 0;

		virtual void find_max_min() { return; }
		virtual void find_z_min(){ return; }
		virtual void check_z_min() { return; }
		virtual bool is_z_calibrated() {return m_z_calibrated; }
		virtual int xy_park() {return -1;}
		virtual int xy_unpark() {return -1;}

};
#endif
