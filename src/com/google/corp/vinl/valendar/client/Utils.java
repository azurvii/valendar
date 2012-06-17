package com.google.corp.vinl.valendar.client;

import com.google.corp.vinl.valendar.shared.AuthenticationException;
import com.google.gwt.i18n.client.DateTimeFormat;
import com.google.gwt.user.client.Window;

public class Utils {

	static public void handleFailure(Throwable caught) {
		if (caught instanceof AuthenticationException) {
			Window.Location.reload();
		} else {
			caught.printStackTrace();
			Window.alert("ERROR: " + caught.getMessage());
		}
	}

	static public String strOfObj(String str) {
		return str == null ? "" : str;
	}

	static public DateTimeFormat getDateFormat() {
		return DateTimeFormat.getFormat("yyyy-MM-dd HH:mm:ss");
	}

	private Utils() {
	}

}
