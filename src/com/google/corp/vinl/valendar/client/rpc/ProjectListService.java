package com.google.corp.vinl.valendar.client.rpc;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Project;
import com.google.gwt.user.client.rpc.RemoteService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;

@RemoteServiceRelativePath("../plistService")
public interface ProjectListService extends RemoteService {

	public void removeProject(Project project);

	public void removeProject(Long id);

	public void addProject(Project project);

	public List<Project> getProjects();

	public Project getProject(Long id);

}
