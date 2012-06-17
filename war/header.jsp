<%@ page import="com.google.appengine.api.users.UserService"%>
<%@ page import="com.google.appengine.api.users.UserServiceFactory"%>

<iframe src="javascript:''" id="__gwt_historyFrame" tabIndex='-1'
	style="position: absolute; width: 0; height: 0; border: 0"></iframe>
<noscript>
	<h1>You need JavaScript enabled to view this page properly.</h1>
</noscript>
<%
	UserService userService = UserServiceFactory.getUserService();
%>
<div class="page-header">
	<b> <%=request.getUserPrincipal().getName()%>
	</b> | <a
		href="<%=userService.createLogoutURL(request.getRequestURL()
					.toString())%>">Log
		out</a>
	<hr />
</div>
