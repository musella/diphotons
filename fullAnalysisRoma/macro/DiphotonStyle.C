void DiphotonStyle()
{
  TStyle* GloStyle;
  GloStyle = gStyle;

  TStyle* DiphotonsStyle = new TStyle("DiphotonsStyle", "DiphotonsStyle");
  gStyle = DiphotonsStyle;


  //----------------------------------------------------------------------------
  // Canvas
  //----------------------------------------------------------------------------
  DiphotonsStyle->SetCanvasBorderMode(  0);
  DiphotonsStyle->SetCanvasBorderSize( 10);
  DiphotonsStyle->SetCanvasColor     (  0);
  DiphotonsStyle->SetCanvasDefH      (600);
  DiphotonsStyle->SetCanvasDefW      (550);
  DiphotonsStyle->SetCanvasDefX      ( 10);
  DiphotonsStyle->SetCanvasDefY      ( 10);


  //----------------------------------------------------------------------------
  // Pad
  //----------------------------------------------------------------------------
  DiphotonsStyle->SetPadBorderMode  (   0);
  DiphotonsStyle->SetPadBorderSize  (  10);
  DiphotonsStyle->SetPadColor       (   0);
  DiphotonsStyle->SetPadBottomMargin(0.20);
  DiphotonsStyle->SetPadTopMargin   (0.08);
  DiphotonsStyle->SetPadLeftMargin  (0.18);
  DiphotonsStyle->SetPadRightMargin (0.05);


  //----------------------------------------------------------------------------
  // Frame
  //----------------------------------------------------------------------------
  DiphotonsStyle->SetFrameFillStyle ( 0);
  DiphotonsStyle->SetFrameFillColor ( 0);
  DiphotonsStyle->SetFrameLineColor ( 1);
  DiphotonsStyle->SetFrameLineStyle ( 0);
  DiphotonsStyle->SetFrameLineWidth ( 2);
  DiphotonsStyle->SetFrameBorderMode( 0);
  DiphotonsStyle->SetFrameBorderSize(10);


  //----------------------------------------------------------------------------
  // Hist
  //----------------------------------------------------------------------------
  DiphotonsStyle->SetHistFillColor(0);
  DiphotonsStyle->SetHistFillStyle(1);
  DiphotonsStyle->SetHistLineColor(1);
  DiphotonsStyle->SetHistLineStyle(0);
  DiphotonsStyle->SetHistLineWidth(1);


  //----------------------------------------------------------------------------
  // Axis
  //----------------------------------------------------------------------------
  DiphotonsStyle->SetLabelFont  (   42, "xyz");
  DiphotonsStyle->SetLabelOffset(0.015, "xyz");
  DiphotonsStyle->SetLabelSize  (0.050, "xyz");
  DiphotonsStyle->SetNdivisions (  505, "xyz");
  DiphotonsStyle->SetTitleFont  (   42, "xyz");
  DiphotonsStyle->SetTitleSize  (0.050, "xyz");

  //  DiphotonsStyle->SetNdivisions ( -503, "y");

  DiphotonsStyle->SetTitleOffset(  1.4,   "x");
  DiphotonsStyle->SetTitleOffset(  1.2,   "y");
  DiphotonsStyle->SetPadTickX   (           1);  // Tick marks on the opposite side of the frame
  DiphotonsStyle->SetPadTickY   (           1);  // Tick marks on the opposite side of the frame


  //----------------------------------------------------------------------------
  // Title
  //----------------------------------------------------------------------------
  DiphotonsStyle->SetTitleBorderSize(    0);
  DiphotonsStyle->SetTitleFillColor (   10);
  DiphotonsStyle->SetTitleAlign     (   12);
  DiphotonsStyle->SetTitleFontSize  (0.045);
  DiphotonsStyle->SetTitleX         (0.560);
  DiphotonsStyle->SetTitleY         (0.860);

  DiphotonsStyle->SetTitleFont(42, "");


  //----------------------------------------------------------------------------
  // Stat
  //----------------------------------------------------------------------------
  DiphotonsStyle->SetOptStat       (1110);
  DiphotonsStyle->SetStatBorderSize(   0);
  DiphotonsStyle->SetStatColor     (  10);
  DiphotonsStyle->SetStatFont      (  42);
  DiphotonsStyle->SetStatX         (0.94);
  DiphotonsStyle->SetStatY         (0.91);

  const Int_t NRGBs = 5;
  const Int_t NCont = 255;

  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  DiphotonsStyle->SetNumberContours(NCont);

  // return();
}
