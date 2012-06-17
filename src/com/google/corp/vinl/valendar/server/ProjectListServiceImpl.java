package com.google.corp.vinl.valendar.server;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Project;
import com.google.corp.vinl.valendar.client.rpc.ProjectListService;
import com.google.gwt.user.server.rpc.RemoteServiceServlet;

@SuppressWarnings("serial")
public class ProjectListServiceImpl extends RemoteServiceServlet implements
		ProjectListService {

	private DAO dao;

	public ProjectListServiceImpl() {
		dao = DAO.get();
	}

	@Override
	public void removeProject(Project project) {
		dao.remove(project);
	}

	@Override
	public void removeProject(Long id) {
		dao.removeProject(id);
	}

	@Override
	public void addProject(Project project) {
		dao.put(project);
	}

	@Override
	public List<Project> getProjects() {
		return dao.getProjects();
	}

	@Override
	public Project getProject(Long id) {
		return dao.getProject(id);
	}

}
