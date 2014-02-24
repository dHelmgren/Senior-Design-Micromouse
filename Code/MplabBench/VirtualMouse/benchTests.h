#ifndef __BENCH_H__
#define __BENCH_H__

/* This file defines all of the bench tests performed on Microtaur,
 * generated by MplabInputGen code. These represent simplified situations 
 * of the mouse while in the maze. 
 * NOTE that MPLAB does not display more than 1024 chars horizontally on
 * one line, but the entire sensor readout is actually there!
 *
 */

// PERFECT CASES: Microtaur is always assumed to be exactly in the center
// of the unit.
//   ____ 
//  |  __
//  | |  
//  | |  
//rom char rightTurnOnlyPerfectClickArray[] = "(5.628,20.755,5.628)#(5.628,20.255,5.628)#(5.628,19.755,5.628)#(5.628,19.255,5.628)#(5.628,18.755,5.628)#(5.628,18.255,5.628)#(5.628,17.755,5.628)#(5.628,17.255,24.402)#(5.628,16.755,23.695)#(5.628,16.255,22.988)#(5.628,15.755,22.281)#(5.628,15.255,21.574)#(5.628,14.755,20.867)#(5.628,14.255,20.160)#(5.628,13.755,19.453)#(5.628,13.255,18.746)#(5.628,12.755,18.038)#(5.628,12.255,17.331)#(5.628,11.755,16.624)#(5.628,11.255,15.917)#(5.628,10.755,15.210)#(5.628,10.255,14.503)#(5.628,9.755,13.796)#(5.628,9.255,13.089)#(5.628,8.755,12.382)#(5.628,8.255,11.674)#(5.628,7.755,10.967)#(5.628,7.255,10.260)#(5.628,6.755,9.553)#(5.628,6.255,8.846)#(5.628,5.755,8.139)#(5.628,5.255,7.432)#(5.628,4.755,6.725)#(5.628,4.255,6.018)#(5.628,3.755,5.311)#(5.628,3.255,4.603)#(5.628,2.755,3.896)#(5.628,36.122,5.628)#(5.628,35.622,5.628)#(5.628,35.122,5.628)#(5.628,34.622,5.628)#(5.628,34.122,5.628)#(5.628,33.622,5.628)#";
rom char rightTurnPerfectClickArray[] = "(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#(7.369,7.369,50.000)#$#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#";
//  ____ 
//  __  |
//    | |
//    | |
//rom char leftTurnOnlyPerfectClickArray[] = "(5.628,20.755,5.628)#(5.628,20.255,5.628)#(5.628,19.755,5.628)#(5.628,19.255,5.628)#(5.628,18.755,5.628)#(5.628,18.255,5.628)#(5.628,17.755,5.628)#(24.402,17.255,5.628)#(23.695,16.755,5.628)#(22.988,16.255,5.628)#(22.281,15.755,5.628)#(21.574,15.255,5.628)#(20.867,14.755,5.628)#(20.160,14.255,5.628)#(19.453,13.755,5.628)#(18.746,13.255,5.628)#(18.038,12.755,5.628)#(17.331,12.255,5.628)#(16.624,11.755,5.628)#(15.917,11.255,5.628)#(15.210,10.755,5.628)#(14.503,10.255,5.628)#(13.796,9.755,5.628)#(13.089,9.255,5.628)#(12.382,8.755,5.628)#(11.674,8.255,5.628)#(10.967,7.755,5.628)#(10.260,7.255,5.628)#(9.553,6.755,5.628)#(8.846,6.255,5.628)#(8.139,5.755,5.628)#(7.432,5.255,5.628)#(6.725,4.755,5.628)#(6.018,4.255,5.628)#(5.311,3.755,5.628)#(4.603,3.255,5.628)#(3.896,2.755,5.628)#(5.628,36.122,5.628)#(5.628,35.622,5.628)#(5.628,35.122,5.628)#(5.628,34.622,5.628)#(5.628,34.122,5.628)#(5.628,33.622,5.628)#";
rom char leftTurnPerfectClickArray[] = "(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#$#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#";
//  _______
//  __   __
//    | |
//    | |
//rom char leftRightTurnWallS0PerfectClickArray[] = "(5.628,20.755,5.628)#(5.628,20.255,5.628)#(5.628,19.755,5.628)#(5.628,19.255,5.628)#(5.628,18.755,5.628)#(5.628,18.255,5.628)#(5.628,17.755,5.628)#(24.402,17.255,5.628)#(23.695,16.755,5.628)#(22.988,16.255,5.628)#(22.281,15.755,5.628)#(21.574,15.255,5.628)#(20.867,14.755,5.628)#(20.160,14.255,5.628)#(19.453,13.755,5.628)#(18.746,13.255,5.628)#(18.038,12.755,5.628)#(17.331,12.255,5.628)#(16.624,11.755,5.628)#(15.917,11.255,5.628)#(15.210,10.755,5.628)#(14.503,10.255,5.628)#(13.796,9.755,5.628)#(13.089,9.255,5.628)#(12.382,8.755,5.628)#(11.674,8.255,5.628)#(10.967,7.755,5.628)#(10.260,7.255,5.628)#(9.553,6.755,5.628)#(8.846,6.255,5.628)#(8.139,5.755,5.628)#(7.432,5.255,5.628)#(6.725,4.755,5.628)#(6.018,4.255,5.628)#(5.311,3.755,5.628)#(4.603,3.255,5.628)#(3.896,2.755,5.628)#(5.628,36.122,5.628)#(5.628,35.622,5.628)#(5.628,35.122,5.628)#(5.628,34.622,5.628)#(5.628,34.122,5.628)#(5.628,33.622,5.628)#";
rom char leftRightTurnPerfectClickArray[] = "(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#$#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#";
//  _
// | |
// | |
// | |
rom char deadEndPerfectClickArray[] = "(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#(7.369,7.369,7.369)#$#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#";
// __| |
// __  |
//   | |
rom char leftStraightTurnPerfectClickArray[] = "(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#(50.000,50.000,7.369)#$#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#";
// | |__
// |  __
// | |
rom char straightRightTurnPerfectClickArray[] = "(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#$#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#";
// __| |__
// __   __
//   | |
rom char leftStraightRightTurnPerfectClickArray[] = "(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#$#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#";

// REAL WORLD CASES; VARIATION 1: Microtaur is still always assumed to be in 
// center of unit, but recognizes tuples even if sees no walls in different
// directions at different times (could be driving at an angle).
// Let the keyword "Unsynced" characterize this variation.
rom char leftRightTurnUnsyncedClickArray[] = "(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,7.369)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#(50.000,7.369,50.000)#$#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#";
rom char leftStraightRightTurnUnsyncedClickArray[] = "(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(7.369,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#(50.000,50.000,50.000)#$#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#(7.369,50.000,7.369)#";

#endif