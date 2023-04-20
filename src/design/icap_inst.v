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






/*
FOR TESTING

KEY (128)
2b7e151628aed2a6abf7158809cf4f3c

IV
000102030405060708090a0b0c0d0e0f

plain
6bc1bee22e409f96e93d7e117393172a
ae2d8a571e03ac9c9eb76fac45af8e51
30c81c46a35ce411e5fbc1191a0a52ef
f69f2445df4f9b17ad2b417be66c3710

result (cypher)
7a	df	a1	d1	61	b9	1d	d1	c2	bb	32	d2	82	a2	29	dc
6f	af	50	68	27	14	3c	20	b1	16	d1	a1	de	0d	bf	ed
ad	7a	43	09	dc	59	ec	2c	55	70	ef	ad	58	1c	34	d6
f3	de	63	83	7c	ef	c6	62	b3	59	d8	41	56	b9	96	af

*/





/*
IN verilog

      nist_aes128_key = 256'h2b7e151628aed2a6abf7158809cf4f3c00000000000000000000000000000000;

      nist_plaintext0 = 128'h6bc1bee22e409f96e93d7e117393172a;
      nist_plaintext1 = 128'hae2d8a571e03ac9c9eb76fac45af8e51;
      nist_plaintext2 = 128'h30c81c46a35ce411e5fbc1191a0a52ef;
      nist_plaintext3 = 128'hf69f2445df4f9b17ad2b417be66c3710;

      nist_iv = 128'h000102030405060708090a0b0c0d0e0f;

      nist_cbc_128_enc_expected0 = 128'h7649abac8119b246cee98e9b12e9197d;
      nist_cbc_128_enc_expected1 = 128'h5086cb9b507219ee95db113a917678b2;
      nist_cbc_128_enc_expected2 = 128'h73bed6b8e3c1743b7116e69e22229516;
      nist_cbc_128_enc_expected3 = 128'h3ff1caa1681fac09120eca307586e1a7;

*/