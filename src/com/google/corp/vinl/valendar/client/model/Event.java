package com.google.corp.vinl.valendar.client.model;

import java.io.Serializable;

import javax.persistence.Entity;
import javax.persistence.Id;

@SuppressWarnings("serial")
@Entity
public class Event implements Serializable, Comparable<Event> {

	@Id
	private Long id;
	private String title;
	private String description;
	private Long offsetMsecs;
	private Long durationMsecs;
	private Long projectId;

	public Event() {
		offsetMsecs = 0L;
		durationMsecs = 1000 * 60 * 60 * 24L;
	}

	public Long getId() {
		return id;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public Long getOffsetMsecs() {
		return offsetMsecs;
	}

	public void setOffsetMsecs(Long offsetMsecs) {
		this.offsetMsecs = offsetMsecs;
	}

	public Long getDurationMsecs() {
		return durationMsecs;
	}

	public void setDurationMsecs(Long durationMsecs) {
		this.durationMsecs = durationMsecs;
	}

	public Long getProjectId() {
		return projectId;
	}

	public void setProjectId(Long projectId) {
		this.projectId = projectId;
	}

	@Override
	public int compareTo(Event o) {
		if (offsetMsecs != null && o.offsetMsecs != null) {
			int offsetComparison = offsetMsecs.compareTo(o.offsetMsecs);
			if (offsetComparison == 0) {
				if (durationMsecs != null && o.durationMsecs != null) {
					return durationMsecs.compareTo(o.durationMsecs);
				}
			} else {
				return offsetComparison;
			}
		}
		return 0;
	}
}
