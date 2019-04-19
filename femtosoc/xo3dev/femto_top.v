module femtosoc_top(
	input wire clk,
	input wire [3:0] pb,
	input wire [3:0] sw,
	inout wire sda, scl, msda, mscl, spi_clk, spi_mosi, spi_miso,
	output wire [7:0] spi_csn,
	output wire [7:0] led
);

parameter integer MEM_WORDS = 8192;

//wire clk;

wire iomem_valid;
reg wb_stb;
wire wb_ack;
wire [7:0] wb_rdata;
reg led_strobe;
reg [7:0] gpo_reg = 8'hA5;
reg [7:0] gpi_reg;
reg iomem_ready = 0;
wire [3:0] iomem_wstrb;
wire [31:0] iomem_addr;
wire [31:0] iomem_wdata;
reg [31:0] iomem_rdata = 32'h0A5;

reg [5:0] reset_cnt = 0;
wire resetn = &reset_cnt;

wire timer_in = pb[2];
wire timer_out;

always @(posedge clk) begin
	reset_cnt <= reset_cnt + !resetn;
end

always @(posedge clk) gpi_reg <= {pb, sw};

assign led = {~gpo_reg[7:1], timer_out};

always @(posedge clk) begin
	if (iomem_valid && (iomem_addr[31:24] == 8'h04) && !wb_ack && !iomem_ready) begin
		wb_stb <= 1'b1;
	end else begin
		wb_stb <= 1'b0;
	end
end
	
always @(posedge clk) begin
	if (iomem_valid) begin
		case (iomem_addr[31:24])
			8'h01: begin
				iomem_ready <= 1'b1;
				if (iomem_wstrb[0]) gpo_reg <= iomem_wdata[7:0];
				iomem_rdata <= {24'h0, gpi_reg};
			end
			8'h04: begin
				iomem_ready <= wb_ack;
				iomem_rdata <= {24'h0, wb_rdata};
			end
			default: begin
				iomem_ready <= 1'b1;
				iomem_rdata <= 32'h00BADADD;
			end
		endcase 
	end else begin
		iomem_ready <= 1'b0;
		iomem_rdata <= 32'hDEADBEEF;
	end
end


// Internal Oscillator
// defparam OSCH_inst.NOM_FREQ = "2.08";// This is the default frequency
// defparam OSCH_inst.NOM_FREQ = "24.18";
// OSCH OSCH_inst( .STDBY(1'b0), // 0=Enabled, 1=Disabled
// .OSC(clk),
// .SEDSTDBY()); // this signal is not required if not using SED


	femtosoc #(
		.MEM_WORDS(MEM_WORDS),
		.MEM_FILE("../firmware/femto_fw.mem"),
		.ENABLE_COMPRESSED(1'b1)
	) soc (
		.clk          (clk         ),
		.resetn       (resetn      ),

		.iomem_valid  (iomem_valid ),
		.iomem_ready  (iomem_ready ),
		.iomem_wstrb  (iomem_wstrb ),
		.iomem_addr   (iomem_addr  ),
		.iomem_wdata  (iomem_wdata ),
		.iomem_rdata  (iomem_rdata )
	);

defparam efb0.UFM_INIT_START_PAGE = 3518;  // 3582 - 64 pages
defparam efb0.UFM_INIT_FILE_NAME = "../firmware/hex5x7bmp.mem" ;
defparam efb0.UFM_INIT_PAGES = 64;
efb efb0 (
	.wb_clk_i( clk ), 
	.wb_rst_i( !resetn ), 
	.wb_cyc_i( wb_stb ), 
	.wb_stb_i( wb_stb ), 
	.wb_we_i( iomem_wstrb[0] ), 
	.wb_adr_i( iomem_addr[9:2] ), 
	.wb_dat_i( iomem_wdata[7:0] ), 
	.wb_dat_o( wb_rdata ), 
	.wb_ack_o( wb_ack ), 
	.i2c1_scl( scl ), 
	.i2c1_sda( sda ), 
	.i2c1_irqo( ), 
	.i2c2_scl( mscl ), 
	.i2c2_sda( msda ), 
	.i2c2_irqo( ), 
	.spi_clk( spi_clk ), 
	.spi_miso( spi_miso ), 
	.spi_mosi( spi_mosi ), 
	.spi_scsn( 1'b1 ), 
	.spi_csn( spi_csn ), 
	.spi_irq( ), 
	.tc_clki( clk ), 
	.tc_rstn( resetn ), 
	.tc_ic( timer_in ), 
	.tc_int( ), 
	.tc_oc( timer_out ), 
	.ufm_sn( 1'b1 ), 
	.wbc_ufm_irq( ) 
	);

endmodule
