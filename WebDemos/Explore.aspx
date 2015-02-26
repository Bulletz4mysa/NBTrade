<%@ Page language="c#" Inherits="WebDemos.Explore" EnableViewState="false" CodeBehind="Explore.aspx.cs" %>
<%@ Register TagPrefix="iewc" Namespace="Microsoft.Web.UI.WebControls" Assembly="Microsoft.Web.UI.WebControls" %>
<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<HTML>
	<HEAD>
		<title>Formula Explore</title>
		<LINK href="m.css" type="text/css" rel="stylesheet">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginwidth="0"
		marginheight="0">
		<form method="post" runat="server">
			<tl:header id="Header" runat="server"></tl:header><tl:demoheader id="DemoHeader" runat="server"></tl:demoheader>
			<b>This demo shows that the Formula script was managed as xml tree.<br>
				Click on Formula script (BBI,CYS etc.) on the tree to show detail </b>
			<table>
				<tr>
					<td vAlign="top" width="200">
						<asp:DropDownList id="ddlFormulaFile" runat="server" AutoPostBack="True" onselectedindexchanged="ddlFormulaFile_SelectedIndexChanged">
							<asp:ListItem Value="basic.fml">basic.fml</asp:ListItem>
							<asp:ListItem Value="Scan.fml">Scan.fml</asp:ListItem>
							<asp:ListItem Value="Test.fml">Test.fml</asp:ListItem>
						</asp:DropDownList>
						<br>
						<iewc:treeview id="FormulaTree" runat="server" CssClass="Tree"></iewc:treeview>
					</td>
					<td vAlign="top" runat="server" id="tdChart">
						<B>Enter Sympol:</B>
						<asp:TextBox id="tbCode" runat="server">MSFT</asp:TextBox>
						<br>
						<b>Formula Parameter:</b><br>
						<asp:Literal id="lParam" runat="server"></asp:Literal>
						<asp:Button id="btnOK" runat="server" Text="Get Chart" onclick="btnOK_Click"></asp:Button>
						<br>
						<br>
						<asp:Literal id="lChart" runat="server"></asp:Literal><br>
						<b>Formula Name:</b><asp:Literal id="lName" runat="server"></asp:Literal><br>
						<b>Formula Full Name:</b><asp:Literal id="lFullName" runat="server"></asp:Literal><br>
						<b>Formula Script:</b> <font color="blue">
							<asp:Literal id="lCode" runat="server"></asp:Literal></font> <b><font color="red">
								<asp:Literal id="lMainView" runat="server" Text="MainView"></asp:Literal></font></b><br>
						<br>
						<b>Formula Description:</b><asp:Literal id="lDescription" runat="server"></asp:Literal><BR>
					</td>
				</tr>
			</table>
			<br>
			<b>Steps:</b>
			<table cellSpacing="1" cellPadding="4" width="640" bgColor="gray">
				<tr bgColor="whitesmoke">
					<td>
						<font color="green">1.Create a Formula script xml file using "Formula compiler" and 
							compile the script to&nbsp;managed dll<br>
							2.Load Xml file to a tree<br>
							3.Launch the selected Formula, show script imformations , set parameters<br>
							4.Get the selected Formula chart </font>
					</td>
				</tr>
			</table>
			<tl:footer id="Footer" runat="server"></tl:footer></form>
	</body>
</HTML>
