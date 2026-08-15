# final-project-seanw_danielm
final-project-seanw_danielm created by GitHub Classroom

This is the implementation for a digital compass display that uses the CMPS2 PMOD to show direction on a FPGA. This was built upon the complete SOC system provided by the class.

In the vivado folder, all that was created was a wrapper for chu_i2c_core and instantiated in slot 14, labeled as CMPS2_slot14. This was to get the specific pmod header pins to interface with the PMOD.

In the vitis folder, the CMPS2 driver code was reused, so the only application code written was for main_video_test.cpp, the ps2 mouse was used to do the compass calibration and the all the graphics were done using the frame buffer core. No OSD or sprite. 
