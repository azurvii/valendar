package com.google.corp.vinl.valendar.client;

import com.google.corp.vinl.valendar.shared.AuthenticationException;
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

	private Utils() {
	}

}
