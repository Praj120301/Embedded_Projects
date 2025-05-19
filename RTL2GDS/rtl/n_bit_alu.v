`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/25/2024 02:43:39 AM
// Design Name: 
// Module Name: n_bit_alu
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

module  n_bit_alu #(parameter n=32)(
                 input [n-1:0] a,
                 input [n-1:0] b,
                 input cin,
                 input ainv,
                 input binv,
                 input [1:0] select,
                 output [n-1:0]result,
                 output cout,
                 output zero,
                 output overflow
                 );
                 wire c[n:0];
                 wire lessval;
                 assign c[0]=cin;
                 assign cout=c[n];
                         alu1bit a0(.a(a[0]),.b(b[0]),.less(lessval),.cin(c[0]),.ainv(ainv),.binv(binv),.select(select),.result(result[0]),.cout(c[1]));//first call
                 generate
                     genvar k;
                     for(k=1;k<n-1;k=k+1)
                     begin : stage 
                        alu1bit a1(.a(a[k]),.b(b[k]),.less(1'b0),.cin(c[k]),.ainv(ainv),.binv(binv),.select(select),.result(result[k]),.cout(c[k+1]));
                        //assume n=32 then last iteration for this loop is k=30
                     end
                 endgenerate
                 alu1bit_lastbit a32 (.a(a[n-1]),.b(b[n-1]),.less(1'b0),.cin(c[n-1]),.ainv(ainv),.binv(binv),.select(select),.result(result[n-1]),.cout(c[n]),.set(lessval));
                 //assume above line is for 31st bit
                 assign zero = ~|result;
                 assign overflow = (~c[n-1] & c[n])|(c[n-1] & ~c[n]);  //c[n-1] cin[n-1] and c[n] cout[n-1]   
               
                 


endmodule
