<%@ page language="java" contentType="text/html; charset=UTF-8"
	pageEncoding="UTF-8"%>
<%
	String pid = request.getParameter("pid");
	if (pid == null || pid.isEmpty()) {
		response.sendRedirect("/plist.jsp");
	}
%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link type="text/css" rel="stylesheet" href="valendar.css" />
<title>Project editor</title>
<script type="text/javascript" language="javascript"
	src="pedit/pedit.nocache.js"></script>
</head>
<body>
	<div id="main"></div>
</body>
</html>