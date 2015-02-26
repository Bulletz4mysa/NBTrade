<%@ Page language="c#" Inherits="WebDemos.DBDemos.Compare" CodeBehind="Compare.aspx.cs" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<HTML>
	<HEAD>
		<title>Easy financial chart </title>
		<META http-equiv="Content-Type" content="text/html; charset=gb2312">
		<LINK href="../m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginwidth="0"
		marginheight="0">
		<form id="wfCompare" method="post" runat="server">
			<tl:header id="Header" runat="server"></tl:header><tl:demoheader id="DemoHeader" runat="server"></tl:demoheader><br>
			<b>Compare Two Stocks:</b><br>
			Ticker1:
			<asp:TextBox id="tbQuote1" runat="server">MSFT</asp:TextBox>
			Ticker2:
			<asp:TextBox id="tbQuote2" runat="server">INTL</asp:TextBox>
			<asp:button id="btnUpdate" runat="server" Text="Update Chart" CssClass="ChartButton" onclick="btnUpdate_Click"></asp:button>
			<br>
			<asp:Literal id="lMsg" runat="server" EnableViewState="False"></asp:Literal>
			<br>
			<asp:DataGrid id="dgStockData" runat="server" BorderColor="#E7E7FF" BorderStyle="None" BorderWidth="1px"
				BackColor="White" CellPadding="2" EnableViewState="False">
				<SelectedItemStyle Font-Bold="True" ForeColor="#F7F7F7" BackColor="#738A9C"></SelectedItemStyle>
				<AlternatingItemStyle BackColor="#F7F7F7"></AlternatingItemStyle>
				<ItemStyle ForeColor="#4A3C8C" BackColor="#E7E7FF"></ItemStyle>
				<HeaderStyle Font-Bold="True" ForeColor="#F7F7F7" BackColor="#4A3C8C"></HeaderStyle>
				<FooterStyle ForeColor="#4A3C8C" BackColor="#B5C7DE"></FooterStyle>
				<PagerStyle HorizontalAlign="Right" ForeColor="#4A3C8C" BackColor="#E7E7FF" Mode="NumericPages"></PagerStyle>
			</asp:DataGrid>
			<table runat="server" visible="false" id="tblChart">
				<tr>
					<td>
						<br>
						<B><font color="red">Click on the chart to add a "cross-hair" price indicator tool.</font></B><br>
						<asp:ImageButton id="ibChart" runat="server" ToolTip="Click on the chart to add a 'cross-hair' price indicator tool."
							Width="700px" Height="800px" onclick="ibChart_Click"></asp:ImageButton><br>
					</td>
				</tr>
			</table>
			<tl:footer id="Footer" runat="server"></tl:footer>
		</form>
	</body>
</HTML>
