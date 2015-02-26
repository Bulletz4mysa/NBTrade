<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Page language="c#" Inherits="WebDemos.Help" CodeBehind="Help.aspx.cs" %>
<HTML>
	<HEAD>
		<title>Easy financial chart </title>
		<meta name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
		<link href="m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginwidth="0"
		marginheight="0">
		<form id="wfHelp" method="post" runat="server">
			<tl:Header id="Header" runat="server"></tl:Header>
			<tl:DemoHeader id="DemoHeader" runat="server"></tl:DemoHeader>
			<br>
			<ul>
				<table>
					<tr>
						<td>
							<li>
								<a href="../Readme.htm">Readme.</a></li><br>
							<br>
							<li>
								<a href="../Help/WindowsDemo.htm">How to use Windows demo.</a></li><br>
							<br>
							<li>
								<a href="../Help/YourOwnFormula.htm">Write you own formulas for easy financial 
									chart.</a></li><br>
							<br>
							<li>
								<a href="../Help/ChartReadme.htm">Parameters of chart.aspx </a>
							</li>
							<br>
							<br>
							<li>
								<a href="../Help/ProgrammingGuide.ppt">Programming guide of easy financial chart.</a></li><br>
							<br>
							<li>
								<a href="../Help/FormulaGuide.htm">Formula script language guide of easy financial 
									chart.</a></li><br>
							<br>
							<li>
								<a href="../Help/Easy Financial Chart.chm">Easy financial chart corelib classes.</a></li><br>
							<br>
							<br>
						</td>
					</tr>
				</table>
			</ul>
			<br>
			<tl:Footer id="Footer" runat="server"></tl:Footer>
		</form>
	</body>
</HTML>
