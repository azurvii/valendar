package com.google.corp.vinl.valendar.server;

import com.google.corp.vinl.valendar.client.rpc.ProjectPublishService;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;

@SuppressWarnings("serial")
public class ProjectPublishServiceImpl extends RemoteServiceServlet implements
		ProjectPublishService {

	private DAO dao;

	public ProjectPublishServiceImpl() {
		dao = DAO.get();
	}

}
