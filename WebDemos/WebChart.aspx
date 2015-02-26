<%@ Page language="c#" Inherits="WebDemos.WebChart" CodeBehind="WebChart.aspx.cs" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<HTML>
	<HEAD>
		<title>Easy financial chart </title>
		<meta name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
		<link href="m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginwidth="0"
		marginheight="0">
		<form id="fmWebChart" method="post" runat="server">
			<tl:Header id="Header" runat="server"></tl:Header>
			<tl:DemoHeader id="DemoHeader" runat="server"></tl:DemoHeader>
			<B>The chart data is downloaded from yahoo finance, and cached at local server.</B>
			<table cellSpacing="1" cellPadding="4" width="680" bgColor="gray">
				<tr bgColor="whitesmoke">
					<td><B>Enter Sympol:</B><asp:textbox id="tbCode" runat="server">MSFT</asp:textbox>
						<asp:button id="btnOK" runat="server" Text="OK"></asp:button>
						<B>Skin:</B>
						<asp:DropDownList id="ddlSkin" runat="server" AutoPostBack="True"></asp:DropDownList><B>Size:</B>
						<asp:DropDownList id="ddlSize" runat="server" AutoPostBack="True">
							<asp:ListItem Value="640" Selected="True">640*480</asp:ListItem>
							<asp:ListItem Value="800">800*600</asp:ListItem>
							<asp:ListItem Value="1024">1024*768</asp:ListItem>
						</asp:DropDownList>
						<b>Width:</b><asp:DropDownList id="ddlWidth" runat="server" AutoPostBack="True">
							<asp:ListItem Value="1" Selected="True">1</asp:ListItem>
							<asp:ListItem Value="1.6">1.6</asp:ListItem>
							<asp:ListItem Value="2">2</asp:ListItem>
						</asp:DropDownList>
					</td>
				</tr>
				<tr bgColor="whitesmoke">
					<td>
						<table cellSpacing="0" cellPadding="0" width="100%">
							<tr>
								<td><B>Range:</B>
									<asp:radiobuttonlist id="rblRange" runat="server" RepeatLayout="Flow" RepeatDirection="Horizontal" AutoPostBack="True">
										<asp:ListItem Value="3m">3m</asp:ListItem>
										<asp:ListItem Value="6m" Selected="True">6m</asp:ListItem>
										<asp:ListItem Value="1y">1y</asp:ListItem>
										<asp:ListItem Value="2y">2y</asp:ListItem>
										<asp:ListItem Value="5y">5y</asp:ListItem>
										<asp:ListItem Value="max">max</asp:ListItem>
									</asp:radiobuttonlist></td>
								<td><B>Type:</B><asp:radiobuttonlist id="rblType" runat="server" RepeatLayout="Flow" RepeatDirection="Horizontal" AutoPostBack="True">
										<asp:ListItem Value="0">Bar1</asp:ListItem>
										<asp:ListItem Value="1">Bar2</asp:ListItem>
										<asp:ListItem Value="2">Line</asp:ListItem>
										<asp:ListItem Value="3" Selected="True">Candle</asp:ListItem>
									</asp:radiobuttonlist>
								</td>
							</tr>
						</table>
					</td>
				</tr>
				<tr bgColor="whitesmoke">
					<td>
						<table cellSpacing="0" cellPadding="0" width="100%">
							<tr>
								<td><B>Moving Avg:</B><asp:checkboxlist id="cblMA" runat="server" RepeatLayout="Flow" RepeatDirection="Horizontal" AutoPostBack="True">
										<asp:ListItem Value="5">5</asp:ListItem>
										<asp:ListItem Value="10">10</asp:ListItem>
										<asp:ListItem Value="20">20</asp:ListItem>
										<asp:ListItem Value="50">50</asp:ListItem>
										<asp:ListItem Value="100">100</asp:ListItem>
										<asp:ListItem Value="200">200</asp:ListItem>
									</asp:checkboxlist>
								</td>
								<td><B>EMA:</B><asp:checkboxlist id="cblEMA" runat="server" RepeatLayout="Flow" RepeatDirection="Horizontal" AutoPostBack="True">
										<asp:ListItem Value="5">5</asp:ListItem>
										<asp:ListItem Value="10" Selected="True">10</asp:ListItem>
										<asp:ListItem Value="20">20</asp:ListItem>
										<asp:ListItem Value="50" Selected="True">50</asp:ListItem>
										<asp:ListItem Value="100">100</asp:ListItem>
										<asp:ListItem Value="200" Selected="True">200</asp:ListItem>
									</asp:checkboxlist>
								</td>
							</tr>
						</table>
					</td>
				</tr>
				<tr bgColor="whitesmoke">
					<td><B>Indicators:</B>
						<asp:checkboxlist id="cblIndicators" runat="server" RepeatLayout="Flow" RepeatDirection="Horizontal"
							AutoPostBack="True">
							<asp:ListItem Value="VOLMA">VOLMA</asp:ListItem>
							<asp:ListItem Value="MACD" Selected="True">MACD</asp:ListItem>
							<asp:ListItem Value="FastSTO">Fast Stoch</asp:ListItem>
							<asp:ListItem Value="SlowSTO">Slow Stoch</asp:ListItem>
							<asp:ListItem Value="ROC">ROC</asp:ListItem>
							<asp:ListItem Value="TRIX">TRIX</asp:ListItem>
							<asp:ListItem Value="RSI">RSI</asp:ListItem>
							<asp:ListItem Value="WVAD">WVAD</asp:ListItem>
							<asp:ListItem Value="ATR">ATR</asp:ListItem>
						</asp:checkboxlist></td>
				</tr>
				<tr bgColor="whitesmoke">
					<td>
					<table width=100%><tr><td>
					<B>Overlays:</B>
						<asp:checkboxlist id="cblOverlays" runat="server" RepeatLayout="Flow" RepeatDirection="Horizontal"
							AutoPostBack="True">
							<asp:ListItem Value="AreaBB">Bollinger Bands</asp:ListItem>
							<asp:ListItem Value="BBI">BBI</asp:ListItem>
							<asp:ListItem Value="SAR" Selected="True">SAR</asp:ListItem>
						</asp:checkboxlist>
						</td><td>
						<B>Scale:</B>
							<asp:radiobuttonlist id="rblScale" runat="server" RepeatLayout="Flow" RepeatDirection="Horizontal" AutoPostBack="True">
								<asp:ListItem Value="0" Selected="True">Linear</asp:ListItem>
								<asp:ListItem Value="1">Log</asp:ListItem>
							</asp:radiobuttonlist>

					</td></tr></table>	
						</td>
				</tr>
				<tr bgColor="whitesmoke">
					<td><B>Compare:</B>
						<asp:Literal id="lCompare" runat="server"></asp:Literal>
						vs
						<asp:TextBox id="tbCompare" runat="server"></asp:TextBox>
						<asp:checkboxlist id="cblCompare" runat="server" RepeatLayout="Flow" RepeatDirection="Horizontal">
							<asp:ListItem Value="^IXIC">Nasdaq</asp:ListItem>
							<asp:ListItem Value="^DJI">Dow</asp:ListItem>
							<asp:ListItem Value="^SPX">S&amp;P</asp:ListItem>
						</asp:checkboxlist>
						<asp:Button id="btnCompare" runat="server" Text="Compare"></asp:Button></td>
				</tr>
				<tr bgcolor="whitesmoke">
					<td>
						<asp:literal id="lChart" runat="server"></asp:literal>
					</td>
				</tr>
			</table>
			<br>
			<B>Steps:</B>
			<table cellSpacing="1" cellPadding="4" width="640" bgColor="gray">
				<tr bgColor="whitesmoke">
					<td><font color="green"> 1.Create stock chart instance<br>
							2.Get stock data from yahoo<br>
							3.Set stock chart properties<br>
							4.Draw extra informations<br>
							5.Create bitmap and output the image stream </font>
					</td>
				</tr>
			</table>
			<br>
			<B>C# Source code:</B>
			<table cellpadding="5" cellspacing="0" border="0">
				<tr>
					<td bgcolor="#f3f3f3" width="100%" valign="top"><nobr> <font size="2" face="Verdana">&nbsp;&nbsp;<font color="green">//Create 
									financial chart instance</font><BR>
								&nbsp;&nbsp;FormulaChart fc = <font color="blue">new</font> FormulaChart();<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Create YahooDataManager , Get stock data from 
									yahoo.</font><BR>
								&nbsp;&nbsp;YahooDataManager ydm = <font color="blue">new</font> YahooDataManager();<BR>
								&nbsp;&nbsp;ydm.CacheRoot = HttpRuntime.AppDomainAppPath+"Cache\\";<BR>
								&nbsp;&nbsp;<font color="blue">string</font> StockCode = 
								Request.Params["Code"];<BR>
								&nbsp;&nbsp;CSVDataProvider DataProvider = (CSVDataProvider)ydm[StockCode];<BR>
								&nbsp;&nbsp;fc[0].Formulas[0].Name = StockCode;<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Add MA lines to main stock view</font><BR>
								&nbsp;&nbsp;<font color="blue">foreach</font>(<font color="blue">string</font> s<font color="blue">
									in </font>Request.Params["MA"].Split(','))<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">if</font> (s!="")<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc[0].AddFormula("FML.MA("+s+")");<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Add EMA lines to main stock view</font><BR>
								&nbsp;&nbsp;<font color="blue">foreach</font>(<font color="blue">string</font> s<font color="blue">
									in </font>Request.Params["EMA"].Split(','))<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">if</font> (s!="")<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc[0].AddFormula("FML.EMA("+s+")");<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Add override lines to main stock view</font><BR>
								&nbsp;&nbsp;<font color="blue">foreach</font>(<font color="blue">string</font> s<font color="blue">
									in </font>Request.Params["OVER"].Split(','))<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">if</font> (s!="")<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc[0].AddFormula("FML."+s);<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Add indicators to stock chart</font><BR>
								&nbsp;&nbsp;<font color="blue">string</font>[] Inds = 
								Request.Params["IND"].Split(',');<BR>
								&nbsp;&nbsp;<font color="blue">foreach</font>(<font color="blue">string</font> s<font color="blue">
									in </font>Inds)<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">if</font> (s!="")<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.AddArea("FML."+s);<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Apply build-in stock chart skin</font><BR>
								&nbsp;&nbsp;fc.SetSkin(FormulaSkin.GetSkinByName(Request.Params["Skin"]));<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Add compare line to other stocks</font><BR>
								&nbsp;&nbsp;<font color="blue">string</font> Comp = Request.Params["COMP"];<BR>
								&nbsp;&nbsp;<font color="blue">if</font> (Comp!=<font color="blue">null</font> &amp;&amp; 
								Comp!="")
								<BR>
								&nbsp;&nbsp;{<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">foreach</font>(<font color="blue">string</font>
								s<font color="blue"> in </font>Comp.Split(','))<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">if</font>
								(s!="")<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;fc[0].AddFormula(<font color="blue">new</font> FML.CompareStock(s,"C"));<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc[0].AxisY.ShowAsPercent = true;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc[0].AxisY.Format = "p1";<BR>
								&nbsp;&nbsp;}<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Set stock chart time period</font><BR>
								&nbsp;&nbsp;fc.EndTime = DateTime.Now.Date;<BR>
								&nbsp;&nbsp;<font color="blue">switch</font> (Request.Params["Range"])
								<BR>
								&nbsp;&nbsp;{<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">c<font color="blue">as</font>e</font>
								"3m":<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.DataCycle 
								= <font color="blue">new</font> DataCycle(DataCycleB<font color="blue">as</font>e.DAY,20);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.StartTime 
								= DateTime.Now.AddMonths(-3).Date;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.AxisXFormat 
								= "MMMdd";<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">break</font>;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">c<font color="blue">as</font>e</font>
								"6m":<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.DataCycle 
								= DataCycle.Month();<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.StartTime 
								= DateTime.Now.AddMonths(-6).Date;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.AxisXFormat 
								= "MMMdd";<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">break</font>;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">c<font color="blue">as</font>e</font>
								"1y":<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.DataCycle 
								= <font color="blue">new</font> DataCycle(DataCycleB<font color="blue">as</font>e.MONTH,2);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.AxisXFormat 
								= "MMMyy";<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;DataProvider.DataCycle 
								= <font color="blue">new</font> DataCycle(DataCycleB<font color="blue">as</font>e.DAY,3);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.StartTime 
								= DateTime.Now.AddYears(-1).Date;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">break</font>;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">c<font color="blue">as</font>e</font>
								"2y":<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.DataCycle 
								= <font color="blue">new</font> DataCycle(DataCycleB<font color="blue">as</font>e.MONTH,4);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.AxisXFormat 
								= "MMMyy";<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;DataProvider.DataCycle 
								= <font color="blue">new</font> DataCycle(DataCycleB<font color="blue">as</font>e.WEEK,1);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.StartTime 
								= DateTime.Now.AddYears(-2).Date;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">break</font>;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">c<font color="blue">as</font>e</font>
								"5y":<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.DataCycle 
								= <font color="blue">new</font> DataCycle(DataCycleB<font color="blue">as</font>e.YEAR,1);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.AxisXFormat 
								= "MMMyy";<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;DataProvider.DataCycle 
								= <font color="blue">new</font> DataCycle(DataCycleB<font color="blue">as</font>e.WEEK,2);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.StartTime 
								= DateTime.Now.AddYears(-5).Date;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">break</font>;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">c<font color="blue">as</font>e</font>
								"max":<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.DataCycle 
								= <font color="blue">new</font> DataCycle(DataCycleB<font color="blue">as</font>e.YEAR,5);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.AxisXFormat 
								= "yyyy";<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;DataProvider.DataCycle 
								= <font color="blue">new</font> DataCycle(DataCycleB<font color="blue">as</font>e.MONTH,1);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.StartTime 
								= DateTime.Now.AddYears(-30).Date;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">break</font>;<BR>
								&nbsp;&nbsp;}<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Set render type : Bar , Candle or Line</font><BR>
								&nbsp;&nbsp;fc[0].StockRenderType = 
								(StockRenderType)int.Parse(Request.Params["Type"]);<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Set Scale type : Normal or Log</font><BR>
								&nbsp;&nbsp;fc[0].ScaleType = (ScaleType)int.Parse(Request.Params["Scale"]);<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Bind stock data</font><BR>
								&nbsp;&nbsp;fc.DataProvider = DataProvider;<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Set stock chart size</font><BR>
								&nbsp;&nbsp;<font color="blue">int </font>Height = 800;<BR>
								&nbsp;&nbsp;<font color="blue">int </font>Width = 600;<BR>
								&nbsp;&nbsp;<font color="blue">switch</font> (Request.Params["Size"])
								<BR>
								&nbsp;&nbsp;{<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">c<font color="blue">as</font>e</font>
								"1":<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Width = 
								640;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Height 
								= 480;
								<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">break</font>;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">c<font color="blue">as</font>e</font>
								"2":<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Width = 
								800;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Height 
								= 600;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">break</font>;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">c<font color="blue">as</font>e</font>
								"3":<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Width = 
								1024;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Height 
								= 768;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">break</font>;<BR>
								&nbsp;&nbsp;}<BR>
								&nbsp;&nbsp;<font color="blue">if</font> (Inds.Length&gt;2)<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Height +=Height/4*(Inds.Length-2);<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Set indicator line width</font><BR>
								&nbsp;&nbsp;<font color="blue">int </font>LineWidth = 
								int.Parse(Request.Params["Width"]);<BR>
								&nbsp;&nbsp;for(<font color="blue">int </font>i=2; i&lt;fc.Areas.Count; i++)<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc[i].LinePen.Width = LineWidth;<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Create stock image to Bitmap</font><BR>
								&nbsp;&nbsp;Response.ContentType = "Image/Png";<BR>
								&nbsp;&nbsp;Bitmap B = fc.GetBitmap(Width,Height,<font color="blue">new</font> Rectangle(0,18,Width,Height-36));<BR>
								&nbsp;&nbsp;<BR>
								&nbsp;&nbsp;<font color="green">//Draw extra text to current stock chart</font><BR>
								&nbsp;&nbsp;Graphics g = Graphics.FromImage(B);<BR>
								&nbsp;&nbsp;Font TextFont = <font color="blue">new</font> Font("Verdana",8);<BR>
								&nbsp;&nbsp;Brush TextBrush = Brushes.Black;<BR>
								&nbsp;&nbsp;Brush BackBrush = Brushes.WhiteSmoke;<BR>
								<BR>
								&nbsp;&nbsp;<font color="blue">string</font> Text;<BR>
								&nbsp;&nbsp;g.FillRectangle(BackBrush,0,0,Width,18);<BR>
								&nbsp;&nbsp;<BR>
								&nbsp;&nbsp;g.FillRectangle(BackBrush,0,Height-18,Width,18);<BR>
								&nbsp;&nbsp;Text = "Copyright 2003 Easychart Inc.";<BR>
								&nbsp;&nbsp;g.DrawString(Text,TextFont,TextBrush,0,Height-16);<BR>
								&nbsp;&nbsp;Text = "http://finance.easychart.net";<BR>
								&nbsp;&nbsp;SizeF Size = g.Me<font color="blue">as</font>ureString(Text,TextFont);<BR>
								&nbsp;&nbsp;g.DrawString(Text,TextFont,TextBrush,Width-Size.Width,Height-16);<BR>
								<BR>
								&nbsp;&nbsp;<font color="blue">double</font>[] dd = DataProvider["DATE"];<BR>
								&nbsp;&nbsp;DateTime D = DateTime.FromOADate(dd[dd.Length-1]);<BR>
								&nbsp;&nbsp;Text = "as of "+D.ToString("dd-MMM-yyyy 
								dddd",DateTimeFormatInfo.InvariantInfo);<BR>
								&nbsp;&nbsp;g.DrawString(Text,TextFont,TextBrush,0,2);<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Create chart stream</font><BR>
								&nbsp;&nbsp;MemoryStream ms = <font color="blue">new</font> MemoryStream();<BR>
								&nbsp;&nbsp;B.Save(ms,ImageFormat.Png);<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Output the chart stream to web browser</font><BR>
								&nbsp;&nbsp;ms.WriteTo(Response.OutputStream);<BR></nobr></FONT></td>
				</tr>
			</table>
			<tl:Footer id="Footer" runat="server"></tl:Footer>
		</form>
	</body>
</HTML>
