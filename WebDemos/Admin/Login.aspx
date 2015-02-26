<%@ Page language="c#" Inherits="WebDemos.Admin.Login" CodeBehind="Login.aspx.cs" %>
<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<HTML>
	<HEAD>
		<title>Login</title>
		<LINK href="../m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body leftmargin="0" marginheight="0" marginwidth="0" topmargin="0" link="#800000" vlink="#800000">
		<form id="LoginForm" method="post" runat="server">
			<tl:Header id="header" runat="server"></tl:Header><tl:DemoHeader id="DemoHeader" runat="server"></tl:DemoHeader>
			<center>
				<br>
				<br>
				<table>
					<tr>
						<td colspan="2">
							<asp:Label id="lMsg" runat="server" ForeColor="Red"></asp:Label>
						</td>
					</tr>
					<tr>
						<td>
							User name
						</td>
						<td>
							<asp:TextBox id="tbUserName" runat="server" Width="200px"></asp:TextBox>
						</td>
					</tr>
					<tr>
						<td>
							Password
						</td>
						<td>
							<asp:TextBox id="tbPassword" runat="server" TextMode="Password" Width="200px"></asp:TextBox>
						</td>
					</tr>
					<tr>
						<td colspan="2">
							<asp:Button id="btnLogin" runat="server" Text="Login" onclick="btnLogin_Click"></asp:Button>
						</td>
					</tr>
				</table>
				<br>
				<br>
			</center>
			<tl:footer id="Footer" runat="server"></tl:footer></form>
		<CENTER></CENTER>
	</body>
</HTML>
