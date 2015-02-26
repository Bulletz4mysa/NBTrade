<%@ Register TagPrefix="tl" TagName="DemoHeader" src="~/Controls/DemoHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Header" src="~/Controls/Header.ascx" %>
<%@ Register TagPrefix="tl" TagName="AdmHeader" src="AdmHeader.ascx" %>
<%@ Register TagPrefix="tl" TagName="Footer" src="~/Controls/Footer.ascx" %>
<%@ Page language="c#" Codebehind="InternetDataToDB.aspx.cs" AutoEventWireup="false" Inherits="WebDemos.InternetDataToDB" EnableViewState="false" %>
<HTML>
	<HEAD>
		<title>Easy financial chart.NET - Demos</title>
		<LINK href="../m.css" type="text/css" rel="stylesheet">
	</HEAD>
	<body vLink="#000080" aLink="#0000ff" link="#000080" leftMargin="0" topMargin="0" marginwidth="0"
		marginheight="0">
		<form id="Default" method="post" runat="server">
			<tl:header id="Header" runat="server"></tl:header><tl:demoheader id="DemoHeader" runat="server"></tl:demoheader>
			<tl:Admheader id="Admheader" runat="server"></tl:Admheader>
			<br>
			<br>
			<table cellSpacing="1" cellPadding="4" bgColor="gray">
				<tr bgColor="whitesmoke">
					<td>
						Exchange :
						<asp:DropDownList id="ddlExchange" runat="server">
							<asp:ListItem Value="NYSE">New York Stock Exchange</asp:ListItem>
							<asp:ListItem Value="AMEX">American Stock Exchange</asp:ListItem>
							<asp:ListItem Value="INDEX">Global Indices</asp:ListItem>
							<asp:ListItem Value="NASDAQ">NASDAQ Stock Exchange</asp:ListItem>
							<asp:ListItem Value="FOREX">Foreign Exchange</asp:ListItem>
							<asp:ListItem Value="HKSE">Hong Kong Stock Exchange</asp:ListItem>
							<asp:ListItem Value="LSE">London Stock Exchange</asp:ListItem>
							<asp:ListItem Value="MLSE">Milan Stock Exchange</asp:ListItem>
							<asp:ListItem Value="OBB">OTC Bulletin Board</asp:ListItem>
							<asp:ListItem Value="PSE">Paris Stock Exchange</asp:ListItem>
							<asp:ListItem Value="SGX">Singapore Stock Exchange</asp:ListItem>
							<asp:ListItem Value="TSE">Toronto Stock Exchange</asp:ListItem>
							<asp:ListItem Value="VSE">Vancouver Stock Exchange</asp:ListItem>
						</asp:DropDownList><hr>
						Import the complete list of symbols from eoddata.com to database.
						<asp:button id="btnGet" runat="server" Text="Import"></asp:button><asp:label id="lExchangeMsg" runat="server" ForeColor="Red"></asp:label>
						<hr>
						Enter stock symbols below, one symbol per line.
						<br>
						<asp:TextBox id="tbRemain" runat="server" TextMode="MultiLine" Width="600px" Height="100px"></asp:TextBox>
						<br>
						<asp:Button id="btnRemain" runat="server" Text="Remain stocks above and delete others"></asp:Button>
						<asp:Button id="btnDelete" runat="server" Text="Delete stocks above"></asp:Button>
						<hr>
						Add symbols , Format : QuoteCode;QuoteName;Exchange:<br>
						<font color="blue">Samples:</font><br>
						ABCW;Anchor BanCorp Wisconsin Inc.;NASDAQ<br>
						ABM;Abm Industries Inc;NYSE<br>
						<asp:TextBox id="tbAddSymbol" runat="server" Height="100px" Width="600px" TextMode="MultiLine"></asp:TextBox>
						<asp:Button id="btnAdd" runat="server" Text="Add"></asp:Button>
						<asp:label id="lAddMsg" runat="server" ForeColor="Red"></asp:label>
						<br>
						<hr>
						<asp:RadioButtonList id="rblMode" runat="server" Width="456px" RepeatDirection="Horizontal" RepeatLayout="Flow">
							<asp:ListItem Value="0" Selected="True">Not downloaded symbols</asp:ListItem>
							<asp:ListItem Value="1">Symbols with data before the date:</asp:ListItem>
						</asp:RadioButtonList>
						<asp:TextBox id="tbDateBefore" runat="server" Width="100px"></asp:TextBox>
						<asp:Label id="lProgress" runat="server"></asp:Label>
						<br>
						Download all historical data from yahoo finance for quotes in the database. 
						This will take a long time.
						<asp:Button id="btnDownload" runat="server" Text="Download"></asp:Button>
						<br>
						<hr>
						Update all quotes of certain day from eoddata.com to database.
						<asp:TextBox id="tbDate" runat="server"></asp:TextBox>
						<asp:Button id="btnUpdateWeb" runat="server" Text="Update"></asp:Button>
						<asp:Button id="btnTestFile" runat="server" Text="TestFile" Visible="False"></asp:Button>
						<asp:Button id="btnCreateFile" runat="server" Text="CreateFile" Visible="False"></asp:Button>
						<hr>
						<asp:TextBox id="tbExport" runat="server" Height="100px" Width="600px" TextMode="MultiLine"></asp:TextBox>
						<asp:Button id="btnExport" runat="server" Text="Export"></asp:Button>
						<hr>
						<asp:Button id="btnUpdateRealtime" runat="server" Text="Update Realtime Data"></asp:Button>
						<asp:Button id="btnClearCache" runat="server" Text="ClearCache"></asp:Button>
					</td>
				</tr>
			</table>
			<tl:footer id="Footer" runat="server"></tl:footer></form>
	</body>
</HTML>
