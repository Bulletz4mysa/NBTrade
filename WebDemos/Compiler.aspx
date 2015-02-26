<%@ Page language="c#" Inherits="WebDemos.Compiler" EnableViewState="false" CodeBehind="Compiler.aspx.cs" %>
<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<HTML>
	<HEAD>
		<title>Tools : Formula Compiler</title>
		<LINK href="m.css" type="text/css" rel="stylesheet">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginwidth="0"
		marginheight="0">
		<form method="post" runat="server">
			<tl:header id="Header" runat="server"></tl:header><tl:demoheader id="DemoHeader" runat="server"></tl:demoheader>
			<b>This is a tool , You can manage Formula program script as a tree, and compile 
				the script to a managed dll.</b>
			<br>
			Some screen spot:<br>
			<br>
			Formula namespace editor:<br>
			<img src="images/compiler-namespace.gif"><br>
			<br>
			Formula program editor:<br>
			<img src="images/compiler-program.gif"><br>
			<br>
			Formula parameter editor:<br>
			<img src="images/compiler-parameter.gif"><br>
			<br>
			<tl:footer id="Footer" runat="server"></tl:footer></form>
	</body>
</HTML>
