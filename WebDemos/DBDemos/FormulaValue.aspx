<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<%@ Page language="c#" Inherits="WebDemos.DBDemos.FormulaValue" CodeBehind="FormulaValue.aspx.cs" %>
<HTML>
	<HEAD>
		<title>Easy financial chart - Scanning</title>
		<LINK href="../m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginheight="0"
		marginwidth="0">
		<form id="wfFormulaValue" method="post" runat="server">
			<tl:header id="Header" runat="server"></tl:header><tl:demoheader id="DemoHeader" runat="server"></tl:demoheader><asp:label id="lChartPage" runat="server" Visible="False">Overview.aspx</asp:label>
			<table>
					<tr>
						<td><asp:label id="lTotal" runat="server"></asp:label></td>
						<td align="right"><asp:label id="lDate" runat="server" Font-Bold="True"></asp:label></td>
					</tr>
					<tr>
						<td colSpan="2"><asp:datagrid id="dgList" runat="server" AutoGenerateColumns="False" AllowPaging="True" BorderColor="DimGray"
								BorderWidth="1px" BackColor="White" CellPadding="2" PageSize="30" ForeColor="Black" AllowCustomPaging="True">
								<FooterStyle BackColor="#CCCC99"></FooterStyle>
								<SelectedItemStyle Font-Bold="True" ForeColor="White" BackColor="#CE5D5A"></SelectedItemStyle>
								<AlternatingItemStyle BackColor="White"></AlternatingItemStyle>
								<ItemStyle BackColor="#F7F7DE"></ItemStyle>
								<HeaderStyle Font-Bold="True" ForeColor="Black" BackColor="#CCCC66"></HeaderStyle>
								<PagerStyle HorizontalAlign="Right" ForeColor="Black" BackColor="#F7F7DE" Mode="NumericPages"></PagerStyle>
							</asp:datagrid><tl:footer id="Footer" runat="server"></tl:footer>
		</form>
		</TD></TR></TABLE>
	</body>
</HTML>
