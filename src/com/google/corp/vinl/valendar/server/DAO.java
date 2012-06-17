package com.google.corp.vinl.valendar.server;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import com.google.appengine.api.datastore.QueryResultIterable;
import com.google.corp.vinl.valendar.client.model.Attendee;
import com.google.corp.vinl.valendar.client.model.Event;
import com.google.corp.vinl.valendar.client.model.Project;
import com.google.corp.vinl.valendar.client.model.Publication;
import com.googlecode.objectify.Key;
import com.googlecode.objectify.Objectify;
import com.googlecode.objectify.ObjectifyService;
import com.googlecode.objectify.util.DAOBase;

public class DAO extends DAOBase {

	// private Logger logger = Logger.getLogger(DAO.class.getName());

	static {
		ObjectifyService.register(Event.class);
		ObjectifyService.register(Project.class);
		ObjectifyService.register(Publication.class);
		ObjectifyService.register(Attendee.class);
	}

	static private DAO dao = new DAO();

	private Objectify ofy;

	private DAO() {
		ofy = ObjectifyService.begin();
	}

	static public DAO get() {
		return dao;
	}

	// delete

	public void remove(Project project) {
		if (project != null) {
			removeProject(project.getId());
		}
	}

	public void remove(Event event) {
		if (event != null) {
			removeEvent(event.getId());
		}
	}

	public void remove(Publication publication) {
		if (publication != null) {
			removePublication(publication.getId());
		}
	}

	public void remove(Attendee attendee) {
		if (attendee != null) {
			removeAttendee(attendee.getId());
		}
	}

	public void removeProject(Long id) {
		if (id != null) {
			Project p = ofy.find(Project.class, id);
			if (p != null) {
				for (Long eid : p.getEventIds()) {
					removeEvent(eid);
				}
				for (Long pubid : p.getPublicationIds()) {
					removePublication(pubid);
				}
				ofy.delete(Project.class, id);
			}
		}
	}

	public void removeEvent(Long id) {
		if (id != null) {
			Event e = ofy.find(Event.class, id);
			if (e != null) {
				Project p = ofy.find(Project.class, e.getProjectId());
				if (p != null) {
					p.removeEvent(id);
					ofy.put(p);
				}
				ofy.delete(Event.class, id);
			}
		}
	}

	public void removePublication(Long id) {
		if (id != null) {
			Publication pub = ofy.find(Publication.class, id);
			if (pub != null) {
				Project proj = ofy.find(Project.class, pub.getProjectId());
				if (proj != null) {
					proj.removePublication(id);
					ofy.put(proj);
				}
				for (Long aid : pub.getAttendeeIds()) {
					Attendee a = ofy.find(Attendee.class, aid);
					if (a != null) {
						a.removePublication(id);
					}
				}
				ofy.delete(Publication.class, id);
			}
		}
	}

	public void removeAttendee(Long id) {
		if (id != null) {
			Attendee a = ofy.find(Attendee.class, id);
			if (a != null) {
				for (Long pid : a.getPublicationIds()) {
					Publication p = ofy.find(Publication.class, pid);
					if (p != null) {
						p.removeAttendee(id);
					}
				}
				ofy.delete(Attendee.class, id);
			}
		}
	}

	// create / update

	public void put(Project project) {
		if (project != null) {
			if (project.getCreationDate() == null) {
				project.setCreationDate(new Date());
			}
			ofy.put(project);
		}
	}

	public void put(Long projectId, Event event) {
		if (projectId != null && event != null) {
			// logger.log(Level.SEVERE, "[DEBUG] putting event " +
			// event.getId());
			Project p = ofy.find(Project.class, projectId);
			if (p != null) {
				event.setProjectId(projectId);
				Key<Event> eventKey = ofy.put(event);
				p.addEvent(ofy.find(eventKey).getId());
				ofy.put(p);
			}
		}
	}

	public void put(Long projectId, List<Event> events) {
		if (projectId != null && events != null && events.size() > 0) {
			if (ofy.find(Project.class, projectId) != null) {
				for (Event e : events) {
					put(projectId, e);
				}
			}
		}
	}

	// retrieve

	public List<Project> getProjects() {
		QueryResultIterable<Project> results = ofy.query(Project.class).fetch();
		List<Project> list = new ArrayList<Project>();
		for (Project p : results) {
			list.add(p);
		}
		return list;
	}

	public Project getProject(Long id) {
		if (id != null) {
			return ofy.find(Project.class, id);
		} else {
			return null;
		}
	}

	public Event getEvent(Long id) {
		if (id != null) {
			return ofy.find(Event.class, id);
		} else {
			return null;
		}
	}

	public List<Event> getEvents(Project project) {
		if (project != null) {
			Project p = getProject(project.getId());
			if (p != null) {
				List<Event> list = new ArrayList<Event>();
				for (Long id : p.getEventIds()) {
					Event e = ofy.find(Event.class, id);
					if (e != null) {
						list.add(e);
					}
				}
				return list;
			}
		}
		return null;
	}

}
