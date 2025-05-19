`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/25/2024 02:15:22 AM
// Design Name: 
// Module Name: n_bit_alu_tb
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


module n_bit_alu_tb();
            parameter n=32;
            reg [n-1:0]a;
            reg [n-1:0]b;
            reg cin;
            reg ainv;
            reg binv;
            reg [1:0] select;
            wire [n-1:0] result;
            wire cout;
            wire overflow;
            wire zero;
            
                 n_bit_alu #(.n(n)) dut(
                 .a(a),
                 .b(b),
                 .cin(cin),
                 .ainv(ainv),
                 .binv(binv),
                 .select(select),
                 .result(result),
                 .cout(cout),
                 .zero(zero),
                 .overflow(overflow)
                 );
                 
initial
                    begin
                        #100 $finish;
                    end
                    
                
                initial
                  begin
                    //bitwise and
                    a=5;
                    b=6;
                    cin=1'b1;
                    ainv=1'b0;
                    binv=1'b1;
                   // select=2'b00;
                    //bitwise or
                    #10
                  //  select=2'b01;
                    //add
                    #10
                    select=2'b10;
                  //  #10
                 //   binv=1'b1;
              //      cin=1'b1;
                    
                  end

endmodule
