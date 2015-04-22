#include "csp_solver_core.h"
#include "csp_solver_util.h"

C_csp_solver::C_csp_solver(C_csp_weatherreader &weather,
	C_csp_collector_receiver &collector_receiver,
	C_csp_power_cycle &power_cycle) : 
	mc_weather(weather), 
	mc_collector_receiver(collector_receiver), 
	mc_power_cycle(power_cycle)
{
	// Inititalize non-reference member data
	m_T_htf_cold_des = m_cycle_W_dot_des = m_cycle_eta_des = m_cycle_q_dot_des = m_cycle_max_frac = m_cycle_cutoff_frac =
		m_cycle_sb_frac = std::numeric_limits<double>::quiet_NaN();

}

void C_csp_solver::init_independent()
{
	mc_weather.init();
	mc_collector_receiver.init();
	mc_power_cycle.init();

	return;
}

void C_csp_solver::init()
{
	init_independent();

	// Get controller values from component models
		// Collector/Receiver
	C_csp_collector_receiver::S_csp_cr_solved_params cr_solved_params;
	mc_collector_receiver.get_design_parameters(cr_solved_params);
	m_T_htf_cold_des = cr_solved_params.m_T_htf_cold_des;	//[K]

		// Power Cycle
	C_csp_power_cycle::S_solved_params solved_params;
	mc_power_cycle.get_design_parameters(solved_params);		
	m_cycle_W_dot_des = solved_params.m_W_dot_des;				//[MW]
	m_cycle_eta_des = solved_params.m_eta_des;					//[-]
	m_cycle_q_dot_des = solved_params.m_q_dot_des;				//[MW]
	m_cycle_max_frac = solved_params.m_cycle_max_frac;			//[-]
	m_cycle_cutoff_frac = solved_params.m_cycle_cutoff_frac;	//[-]
	m_cycle_sb_frac = solved_params.m_cycle_sb_frac;			//[-]

}


void C_csp_solver::simulate()
{
	size_t hour = 0;					//[hr] hardcode simulation to start at first of year, for now
	size_t hour_end = 8760;				//[hr] hardcode simulation to run through entire year, for now
	mc_sim_info.m_step = 3600.0;		//[hr] hardcode steps = 1 hr, for now

	C_csp_solver_htf_state cr_htf_state;
	C_csp_collector_receiver::S_csp_cr_inputs cr_inputs;
	C_csp_collector_receiver::S_csp_cr_outputs cr_outputs;

	bool is_rec_su_allowed = true;
	bool is_pc_su_allowed = true;
	bool is_pc_sb_allowed = true;

	int cr_operating_state = C_csp_collector_receiver::E_csp_cr_modes::OFF;
	int pc_operating_state = C_csp_power_cycle::E_csp_power_cycle_modes::OFF;

	bool is_est_rec_output_useful = false;

	while( hour < 8760 )
	{
		mc_sim_info.m_time = mc_sim_info.m_step*(hour + 1);
		
		// Get collector/receiver & power cycle operating states


		// Get weather at this timestep. Should only be called once per timestep. (Except converged() function)
		mc_weather.timestep_call(mc_sim_info);
		
		// Solve collector/receiver with design inputs and weather to estimate output
			// May replace this call with a simple proxy model later...
		cr_htf_state.m_temp_in = m_T_htf_cold_des - 273.15;		//[C], convert from [K]
		cr_inputs.m_field_control = 1.0;						//[-] no defocusing for initial simulation
		mc_collector_receiver.call(mc_weather.ms_outputs,
			cr_htf_state,
			cr_inputs,
			cr_outputs,
			mc_sim_info);


		double q_dot_cr_output = cr_outputs.m_q_thermal;		//[MW]





		// Timestep solved: run post-processing, converged()
	
	}

}