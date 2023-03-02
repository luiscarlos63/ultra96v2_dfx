`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/22/2023 11:35:26 PM
// Design Name: 
// Module Name: icap_inst
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module icap_inst(
    input CLK,
    output out_I,
    output out_O,
//    output out_clk,
  (* X_INTERFACE_INFO = "xilinx.com:interface:icap:1.0 ICAP avail" *)   output        AVAIL,
  (* X_INTERFACE_INFO = "xilinx.com:interface:icap:1.0 ICAP o" *)       output [31:0] O,
  (* X_INTERFACE_INFO = "xilinx.com:interface:icap:1.0 ICAP prdone" *)  output        PRDONE,
  (* X_INTERFACE_INFO = "xilinx.com:interface:icap:1.0 ICAP prerror" *) output        PRERROR,
  (* X_INTERFACE_INFO = "xilinx.com:interface:icap:1.0 ICAP csib" *)    input         CSIB,
  (* X_INTERFACE_INFO = "xilinx.com:interface:icap:1.0 ICAP i" *)       input [31:0]  I,
  (* X_INTERFACE_INFO = "xilinx.com:interface:icap:1.0 ICAP rdwrb" *)   input         RDWRB
  );

   ICAPE3 u_icap (
      .AVAIL(AVAIL),          // 1-bit output: Availability status of ICAP
      .O(O),                  // 32-bit output: Configuration data output bus
      .PRDONE(PRDONE),        // 1-bit output: Indicates completion of Dynamic Function eXchange
      .PRERROR(PRERROR),      // 1-bit output: Indicates Error during Dynamic Function eXchange
      .CLK(CLK),              // 1-bit input: Clock input
      .CSIB(CSIB),            // 1-bit input: Active-Low ICAP enable
      .I(I),  // 32-bit input: Configuration data input bus
      .RDWRB(RDWRB)           // 1-bit input: Read/Write Select input
   );

assign out_I = I;
assign out_O = O;
//ssign out_clk = CLK;
endmodule



