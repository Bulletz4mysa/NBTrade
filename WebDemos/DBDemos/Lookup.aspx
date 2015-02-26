<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<%@ Page language="c#" Inherits="WebDemos.DBDemos.Lookup" CodeBehind="Lookup.aspx.cs" %>
<HTML>
	<HEAD>
		<title>Easy financial chart - Symbol lookup</title>
		<link href="../m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginwidth="0"
		marginheight="0">
		<form id="wfLookup" method="post" runat="server">
			<tl:Header id="Header" runat="server"></tl:Header>
			<tl:DemoHeader id="DemoHeader" runat="server"></tl:DemoHeader>
			<br>
			Lookup symbols :
			<asp:TextBox id="tbKey" runat="server" Width="160px"></asp:TextBox>
			<asp:DropDownList id="ddlExchange" runat="server" DataTextField="Text" DataValueField="Value"></asp:DropDownList>
			<asp:Button id="btnSearch" runat="server" Text="Lookup" CssClass="ChartButton" onclick="btnSearch_Click"></asp:Button>
			<br>
			<tl:Footer id="Footer" runat="server"></tl:Footer>
		</form>
	</body>
</HTML>
