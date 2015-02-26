<%@ Page language="c#" Inherits="WebDemos.Admin.UpdatePreScan" CodeBehind="UpdatePreScan.aspx.cs" %>
<%@ Register TagPrefix="tl" TagName="AdmHeader" src="AdmHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<HTML>
	<HEAD>
		<title>Easy financial chart.NET - Demos</title>
		<LINK href="../m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginwidth="0"
		marginheight="0">
		<form id="wfUpdatePreScan" method="post" runat="server">
			<tl:header id="Header" runat="server"></tl:header><tl:demoheader id="DemoHeader" runat="server"></tl:demoheader>
			<tl:Admheader id="Admheader" runat="server"></tl:Admheader>
			<br>
			<br>
			Pre-scan formulas:
			<asp:Label id="lStatus" runat="server" ForeColor="Red" Font-Bold="True"></asp:Label>
			<br>
			<asp:TextBox id="tbPreScan" runat="server" TextMode="MultiLine" Width="680px" Height="504px"></asp:TextBox>
			<br>
			<asp:Button id="btnSave" runat="server" Text="Save" onclick="btnSave_Click"></asp:Button>
			<asp:Button id="btnPreScan" runat="server" Text="Start scanning" onclick="btnPreScan_Click"></asp:Button><BR>
			<tl:footer id="Footer" runat="server"></tl:footer>
		</form>
	</body>
</HTML>
