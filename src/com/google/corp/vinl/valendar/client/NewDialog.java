package com.google.corp.vinl.valendar.client;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyDownHandler;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.DialogBox;
import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.HorizontalPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.VerticalPanel;

public class NewDialog extends DialogBox {

	// final private ProjectListEntry entry;

	// private String name;
	// private String type;
	TextBox nameText;
	Button addButton;
	Button cancelButton;

	public NewDialog(final EntryThatCanCreate entry) {
		// initWidget(uiBinder.createAndBindUi(this));
		// this.entry = entry;
		VerticalPanel vpanel = new VerticalPanel();
		vpanel.add(new Label("Name"));
		nameText = new TextBox();
		addButton = new Button("Create");
		final NewDialog d = this;
		addButton.addClickHandler(new ClickHandler() {
			@Override
			public void onClick(ClickEvent event) {
				d.hide();
				entry.createNew(nameText.getText());
				nameText.setText("");
			}
		});
		cancelButton = new Button("Cancel");
		cancelButton.addClickHandler(new ClickHandler() {
			@Override
			public void onClick(ClickEvent event) {
				d.hide();
				nameText.setText("");
			}
		});
		nameText.addKeyDownHandler(new KeyDownHandler() {
			@Override
			public void onKeyDown(KeyDownEvent event) {
				if (event.getNativeKeyCode() == KeyCodes.KEY_ENTER
						&& !nameText.getText().isEmpty()) {
					addButton.click();
				}
				if (event.getNativeKeyCode() == KeyCodes.KEY_ESCAPE) {
					cancelButton.click();
				}
			}
		});
		HorizontalPanel hpanel = new HorizontalPanel();
		hpanel.add(addButton);
		hpanel.add(cancelButton);
		vpanel.add(nameText);
		vpanel.add(hpanel);
		HTML notice = new HTML();
		notice.setHTML("Creation takes a while.<br />"
				+ "Refresh the list in a second.");
		vpanel.add(notice);
		this.add(vpanel);
	}

	// public String getName() {
	// return name;
	// }
	//
	// public void setName(String name) {
	// this.name = name;
	// }
	//
	// public String getType() {
	// return type;
	// }
	//
	// public void setType(String type) {
	// this.type = type;
	// }
}
