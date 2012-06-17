package com.google.corp.vinl.valendar.client.rpc;

import java.util.List;

import com.google.corp.vinl.valendar.client.model.Project;
import com.google.gwt.user.client.rpc.AsyncCallback;

public interface ProjectListServiceAsync {

	void addProject(Project project, AsyncCallback<Void> callback);

	void getProjects(AsyncCallback<List<Project>> callback);

	void removeProject(Project project, AsyncCallback<Void> callback);

	void removeProject(Long id, AsyncCallback<Void> callback);

	void getProject(Long id, AsyncCallback<Project> callback);

}
