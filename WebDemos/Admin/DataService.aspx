<%@ Page language="c#" Codebehind="DataService.aspx.cs" AutoEventWireup="false" Inherits="WebDemos.DataService" %>
<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="AdmHeader" src="AdmHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<HTML>
	<HEAD>
		<title>Easy financial chart.NET - Demos</title>
		<LINK href="../m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginheight="0"
		marginwidth="0">
		<form id="Default" method="post" runat="server">
			<tl:header id="Header" runat="server"></tl:header><tl:demoheader id="DemoHeader" runat="server"></tl:demoheader>
			<tl:admheader id="Admheader" runat="server"></tl:admheader>
			<table cellSpacing="1" cellPadding="4" bgColor="gray">
				<tr bgColor="whitesmoke">
					<td>
						<asp:DropDownList id="ddlDataFeed" runat="server" Visible="False"></asp:DropDownList><asp:dropdownlist id="ddlExchange" runat="server">
							<asp:ListItem Value="AMEX">American Stock Exchange </asp:ListItem>
							<asp:ListItem Value="NasdaqNM">Nasdaq Stock Exchange NM</asp:ListItem>
							<asp:ListItem Value="NasdaqSC">Nasdaq Stock Exchange SC</asp:ListItem>
							<asp:ListItem Value="NYSE">NYSE Stock Exchanges</asp:ListItem>
							<asp:ListItem Value="OTC BB">OTC Bulletin Board Market</asp:ListItem>
							<asp:ListItem Value="Shanghai">Shanghai Stock Exchange</asp:ListItem>
							<asp:ListItem Value="Shenzhen">Shenzhen Stock Exchange</asp:ListItem>
							<asp:ListItem Value="Toronto">Toronto Stock Exchange (TOR)</asp:ListItem>
							<asp:ListItem Value="Vancouver">TSX Venture Exchange (CVE)</asp:ListItem>
							<asp:ListItem Value="^">Global Indices</asp:ListItem>
						</asp:dropdownlist>
						<asp:label id="lExchangeMsg" runat="server" ForeColor="Red"></asp:label>
						<hr>
						Import the complete list of symbols from data.easychart.net to database.
						<asp:button id="btnImport" runat="server" Text="Import"></asp:button>
						<hr>
						Update all quotes of certain day from data.easychart.net to database.
						<asp:textbox id="tbDate" runat="server"></asp:textbox>
						<asp:Button id="btnUpdateWeb" runat="server" Text="Update"></asp:Button>
						<hr>
						<asp:Button id="btnDeleteExchange" runat="server" Text="Delete selected exchange"></asp:Button>
						<hr>
						<asp:Button id="btnFixWrongData" runat="server" Text="Fix wrong data"></asp:Button>
						<asp:Label id="lFixProgress" runat="server"></asp:Label>
						<asp:DataGrid id="dgWrongList" runat="server" BorderColor="#E7E7FF" BorderStyle="None" BorderWidth="1px"
							BackColor="White" CellPadding="3" AutoGenerateColumns="False" DataKeyField="ConditionId">
							<FooterStyle ForeColor="#4A3C8C" BackColor="#B5C7DE"></FooterStyle>
							<SelectedItemStyle Font-Bold="True" ForeColor="#F7F7F7" BackColor="#738A9C"></SelectedItemStyle>
							<AlternatingItemStyle BackColor="#F7F7F7"></AlternatingItemStyle>
							<ItemStyle ForeColor="#4A3C8C" BackColor="#E7E7FF"></ItemStyle>
							<HeaderStyle Font-Bold="True" ForeColor="#F7F7F7" BackColor="#4A3C8C"></HeaderStyle>
							<Columns>
								<asp:HyperLinkColumn Text="List" DataNavigateUrlField="ConditionId" DataNavigateUrlFormatString="../DBDemos/StockList.aspx?ConditionId={0}"
									HeaderText="List"></asp:HyperLinkColumn>
								<asp:ButtonColumn Text="Delete" HeaderText="Delete" CommandName="Delete"></asp:ButtonColumn>
								<asp:BoundColumn DataField="Description" HeaderText="Description"></asp:BoundColumn>
								<asp:BoundColumn DataField="Count" HeaderText="Count"></asp:BoundColumn>
							</Columns>
							<PagerStyle HorizontalAlign="Right" ForeColor="#4A3C8C" BackColor="#E7E7FF" Mode="NumericPages"></PagerStyle>
						</asp:DataGrid>
					</td>
				</tr>
			</table>
			<tl:footer id="Footer" runat="server"></tl:footer></form>
	</body>
</HTML>
