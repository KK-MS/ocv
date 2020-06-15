# Ground server standalone with odometry functionalities


### Review comments:
ZIM-236. Standalone GT server and odometry.

Code has hardcode value for ROI in gt_server_embedded_process_frame.cpp
```javascript
#if 1
	// Manual ROI create
	ROI_x1 = 420;
	ROI_y1 = 240;
	ROI_x2 = 210;
	ROI_y2 = 100;
#endif
```
1. Important that we have code without manual ROI and 
2. Test the application with the manual ROI.
--------------:
3. There is a dependency on frame number: 
int i = 8955;         // save_prcoess_frame
This will make exe not to be used in other cases.
design is to get the frame name from the mapped csv and 
can be used for output frame name/number also.
----------------
4. START_X, START_Y: TODO: get after the value from calibration of the setup. It should also have tolerance range to compensate vibration.
5. ROI_x2, ROI_y2 : TODO: based on the image quality and processing output. 
This can change if the processing output confident is not sufficient.
----
6. remove end spaces
7. expand tab to spaces
8. add comments above functions.
9. check for 6 arguments? do we need 6? add comment
10. Create a function to Read csv file and fill the structure. 
Currently run_app is overloaded
11. Few variable to be renamed: TODO: In development activity.
12. in function bearing_angle90deg() why we are shifting angle by 90 degree? add comment 
