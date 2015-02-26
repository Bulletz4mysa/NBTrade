<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Page language="c#" Inherits="WebDemos.Formula" ValidateRequest=false CodeBehind="Formula.aspx.cs" %>
<HTML>
	<HEAD>
		<title>Easy financial chart </title>
		<meta name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
		<link href="m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginwidth="0"
		marginheight="0">
		<form id="wfFormula" method="post" runat="server">
			<tl:Header id="Header" runat="server"></tl:Header>
			<tl:DemoHeader id="DemoHeader" runat="server"></tl:DemoHeader>
			<B>This demo shows that Easy financial chart can compile Formula script on the fly.</B>
			<table cellSpacing="1" cellPadding="4" width="640" bgColor="gray">
				<tr bgcolor="whitesmoke">
					<td colspan="2">
						<B>Enter Sympol:</B>
						<asp:TextBox id="tbCode" runat="server">MSFT</asp:TextBox>
					</td>
				</tr>
				<tr bgColor="whitesmoke">
					<td>
						<b>Formula Name:</b>
						<asp:TextBox id="tbFormulaName" runat="server" Width="200px">Test</asp:TextBox>
						<br>
						<B>Formula Script:</B>(See online document for more detail)
						<asp:TextBox id="tbProgramScript" runat="server" TextMode="MultiLine" Width="488px" Height="200px">
DIFF:EMA(CLOSE,SHORT)-EMA(CLOSE,LONG);                   
DEA :EMA(DIFF,MID);                   
MACD:DIFF-DEA,COLORSTICK;
						</asp:TextBox>
						<br>
						<b>Parameter:</b>
						<table>
							<tr>
								<td>Name</td>
								<td>Default Value</td>
								<td>Minimum Value</td>
								<td>Maximum Value</td>
							</tr>
							<tr>
								<td>
									1.<asp:TextBox id="tbParamName1" runat="server" Width="80px">LONG</asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbDefValue1" runat="server" Width="80px">26</asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbMinValue1" runat="server" Width="80px">20</asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbMaxValue1" runat="server" Width="80px">100</asp:TextBox>
								</td>
							</tr>
							<tr>
								<td>
									2.<asp:TextBox id="tbParamName2" runat="server" Width="80px">SHORT</asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbDefValue2" runat="server" Width="80px">12</asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbMinValue2" runat="server" Width="80px">5</asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbMaxValue2" runat="server" Width="80px">19</asp:TextBox>
								</td>
							</tr>
							<tr>
								<td>
									3.<asp:TextBox id="tbParamName3" runat="server" Width="80px">MID</asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbDefValue3" runat="server" Width="80px">9</asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbMinValue3" runat="server" Width="80px">2</asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbMaxValue3" runat="server" Width="80px">20</asp:TextBox>
								</td>
							</tr>
							<tr>
								<td>
									4.<asp:TextBox id="tbParamName4" runat="server" Width="80px"></asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbDefValue4" runat="server" Width="80px"></asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbMinValue4" runat="server" Width="80px"></asp:TextBox>
								</td>
								<td>
									<asp:TextBox id="tbMaxValue4" runat="server" Width="80px"></asp:TextBox>
								</td>
							</tr>
						</table>
						<asp:Button id="btnOK" runat="server" Text="OK" onclick="btnOK_Click"></asp:Button>
					</td>
					<td>
						<asp:literal id="lChart" runat="server"></asp:literal>
					</td>
				</tr>
			</table>
			<br>
			<B>Steps:</B>
			<table cellSpacing="1" cellPadding="4" width="640" bgColor="gray">
				<tr bgColor="whitesmoke">
					<td>
						<font color="green">1.Create Formula namespace, add program and parameters to it<br>
							2.Compile the Formula namespace on the fly<br>
							3.Get stock data from yahoo<br>
							4.Create temp image on the server<br>
							5.Show image or error messages<br>
						</font>
					</td>
				</tr>
			</table>
			<br>
			<B>C# source code</B>
			<table cellpadding="5" cellspacing="0" border="0">
				<tr>
					<td bgcolor="#f3f3f3" width="100%" valign="top"><nobr> <font size="2" face="Verdana">&nbsp;&nbsp;<font color="green">//Create 
									a Formula namespace</font><BR>
								&nbsp;&nbsp;FormulaSpace fms = <font color="blue">new</font> FormulaSpace("FML");<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Create a Formula program , set Formula name and 
									script code on the fly</font><BR>
								&nbsp;&nbsp;FormulaProgram fp = <font color="blue">new</font> FormulaProgram();<BR>
								&nbsp;&nbsp;fp.Name = tbFormulaName.Text;<BR>
								&nbsp;&nbsp;fp.Code = tbProgramScript.Text;<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Add the script program to the Formula namespace</font><BR>
								&nbsp;&nbsp;fms.Programs.Add(fp);<BR>
								<BR>
								&nbsp;&nbsp;<font color="green">//Add parameters to Formula program</font><BR>
								&nbsp;&nbsp;for(<font color="blue">int </font>i= 1;i&lt;5; i++)
								<BR>
								&nbsp;&nbsp;{<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">if</font> (Request.Form["tbParamName"+i]!="")
								<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fp.Params.Add(<font color="blue">new</font>
								FormulaParam(<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Request.Form["tbParamName"+i],<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;float.Parse(Request.Form["tbDefValue"+i]),<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;float.Parse(Request.Form["tbMinValue"+i]),<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;float.Parse(Request.Form["tbMaxValue"+i])));<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}<BR>
								&nbsp;&nbsp;}<BR>
								<BR>
								&nbsp;&nbsp;<font color="blue">try</font>
								<BR>
								&nbsp;&nbsp;{<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="green">//Compile the Formula 
									script on the fly</font><BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Assembly a = 
								fms.Compile(HttpRuntime.BinDirectory);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;FormulaB<font color="blue">as</font>e 
								fb = FormulaB<font color="blue">as</font>e.GetFormulaByName(a,fms.Name+"."+fp.Name);<BR>
								<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="green">//Create 
									YahooDataManager , Get stock data from yahoo.</font><BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;YahooDataManager ydm = <font color="blue">
									new</font> YahooDataManager();<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ydm.CacheRoot = 
								HttpRuntime.AppDomainAppPath+"Cache\\";<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;CSVDataProvider DataProvider = 
								(CSVDataProvider)ydm[tbCode.Text];<BR>
								&nbsp;&nbsp;<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="green">//Create financial 
									chart instance</font><BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;FormulaChart fc = <font color="blue">new</font>
								FormulaChart();<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.AddArea(fb);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.DataProvider = DataProvider;<BR>
								<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="green">//Create a temp image 
									on the web server</font><BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;FileStream fs = 
								File.OpenWrite(HttpRuntime.AppDomainAppPath+"Demo\\Temp.png");<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fc.SaveToStream(fs,440,440,<font color="blue">new</font>
								Rectangle(0,0,440,440),ImageFormat.Png);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fs.Close();<BR>
								<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="green">//Show the temp image 
									just created</font><BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;lChart.Text = 
								"&lt;imgsrc=Temp.Png?R="+Rnd.Next(100000)+"&gt;";<BR>
								&nbsp;&nbsp;}<BR>
								&nbsp;&nbsp;<font color="blue">catch</font> (FormulaErrorException fee)<BR>
								&nbsp;&nbsp;{<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="green">//Show the compile 
									result if the script has some errors</font><BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;lChart.Text = "";<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">foreach</font>(System.CodeDom.Compiler.CompilerError 
								ce<font color="blue"> in </font>fee.ces)
								<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;FormulaProgram 
								fpa = fms.GetProgramByLineNum(ce.Line);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="blue">if</font>
								(fpa!=<font color="blue">null</font>)<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fpa.AdjustErrors(ce);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;lChart.Text 
								+= <font color="blue">string</font>.Format("Name:{0} line:{1} column:{2} 
								error:{3} {4}",fpa.Name,ce.Line,ce.Column,ce.ErrorNumber,ce.ErrorText);<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}<BR>
								&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;lChart.Text +="";<BR>
								&nbsp;&nbsp;}<BR></nobr></FONT></td>
				</tr>
			</table>
			<tl:Footer id="Footer" runat="server"></tl:Footer>
		</form>
	</body>
</HTML>
