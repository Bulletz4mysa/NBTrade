<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="AdmHeader" src="AdmHeader.ascx" %>
<%@ Page language="c#" Inherits="WebDemos.Admin.UpdateFormulaValue" CodeBehind="UpdateFormulaValue.aspx.cs" %>
<HTML>
	<HEAD>
		<title>Easy financial chart.NET - Demos</title>
		<LINK href="../m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginheight="0"
		marginwidth="0">
		<form id="Default" method="post" runat="server">
			<tl:header id="Header" runat="server"></tl:header><tl:demoheader id="DemoHeader" runat="server"></tl:demoheader><tl:admheader id="Admheader" runat="server"></tl:admheader><br>
			<br>
			Auto pull formulas:
			<asp:label id="lStatus" runat="server" Font-Bold="True" ForeColor="Red"></asp:label><br>
			<asp:textbox id="tbPullFormulas" runat="server" Height="488px" Width="616px" TextMode="MultiLine"></asp:textbox><br>
			<asp:button id="btnSave" runat="server" Text="Save" onclick="btnSave_Click"></asp:button><asp:button id="btnUpdate" runat="server" Text="Update formula values" onclick="btnUpdate_Click"></asp:button><br>
			<tl:footer id="Footer" runat="server"></tl:footer></form>
	</body>
</HTML>
