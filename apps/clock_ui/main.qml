import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import Sequencer 1.0

ApplicationWindow {
    visible: true
    width: 900
    height: 260
    title: qsTr("MIDI Clock")

    Backend
    {
        id: backend
    }

    ColumnLayout
    {
        anchors.centerIn: parent

        GridLayout {
            columns: 3
            id: midi_clock_button_layout

            Button {
                id: play_button
                text: qsTr("Play")

                onClicked: {
                    backend.start_clock()
                }
            }

            Button {
                id: stop_button
                text: qsTr("Stop")

                onClicked: {
                    backend.stop_clock()
                }
            }

            Button {
                id: reset_button
                text: qsTr("Reset")

                onClicked: {
                    backend.reset_clock()
                }
            }

            SpinBox {
                id: bpm_spinbox
                from: 0
                to: 1000
                value: 120
                editable: true
                Layout.columnSpan: 2

                textFromValue: function(value, locale) {
                    return Number(value).toLocaleString(locale, 'f', 0) + " bpm";
                }

                valueFromText: function(text, locale) {
                    return Number.fromLocaleString(locale, text.replace(" bpm", ""));
                }

                onValueModified: {
                    backend.set_clock_tempo(bpm_spinbox.value)
                }
            }

            SpinBox {
                id: port_spinbox
                from: 0
                to: items.length - 1
                value: 0
                Layout.columnSpan: 3

                property var items: backend.available_ports().split(";")

                textFromValue: function(value) {
                    return items[value];
                }

                valueFromText: function(text) {
                    for (var i = 0; i < items.length; ++i) {
                        if (items[i].toLowerCase().indexOf(text.toLowerCase()) === 0)
                            return i
                    }
                    return sb.value
                }

                onValueChanged: {
                    var result = backend.open_port(value)
                    if(items[value] === "no port selected") {
                        status_label.text = "Disconnected"
                    }
                    else {
                        if(result === false) {
                            status_label.text = "Could not open " + items[value]
                        }
                        else {
                            status_label.text = "Successfully connected to " + items[value]
                        }
                    }
                }
            }
        }

        RowLayout {
            id: song_position_pointer_box_layout

            Label {
                id: song_position_pointer_box_text_label
                text: "Song Position Pointer"
            }

            SpinBox {
                id: song_position_pointer_box
                from: 0
                to: 16383
                editable: true

                onValueModified: {
                    backend.set_song_position_pointer(song_position_pointer_box.value)
                }
            }

            Label {
                id: in_beats_label
                text: qsTr(" in beats: " + Number(song_position_pointer_box.value/4).toLocaleString(Qt.locale("en_EN"), 'f', 2))
            }
        }

        RowLayout {
            id: status_layout

            Label {
                id: status_label_header
                text: " -> "
            }

            Label {
                id: status_label
                color: "black"
                text: qsTr("")
            }
        }

        RowLayout {
            id: sequencer_layout

            CheckBox {
                id: step_0

                onClicked: {
                    backend.set_step(0, checked);
                }
            }

            CheckBox {
                id: step_1

                onClicked: {
                    backend.set_step(1, checked);
                }
            }

            CheckBox {
                id: step_2

                onClicked: {
                    backend.set_step(2, checked);
                }
            }

            CheckBox {
                id: step_3

                onClicked: {
                    backend.set_step(3, checked);
                }
            }

            CheckBox {
                id: step_4

                onClicked: {
                    backend.set_step(4, checked);
                }
            }

            CheckBox {
                id: step_5

                onClicked: {
                    backend.set_step(5, checked);
                }
            }

            CheckBox {
                id: step_6

                onClicked: {
                    backend.set_step(6, checked);
                }
            }

            CheckBox {
                id: step_7

                onClicked: {
                    backend.set_step(7, checked);
                }
            }

            CheckBox {
                id: step_8

                onClicked: {
                    backend.set_step(8, checked);
                }
            }

            CheckBox {
                id: step_9

                onClicked: {
                    backend.set_step(9, checked);
                }
            }

            CheckBox {
                id: step_10

                onClicked: {
                    backend.set_step(10, checked);
                }
            }

            CheckBox {
                id: step_11

                onClicked: {
                    backend.set_step(11, checked);
                }
            }

            CheckBox {
                id: step_12

                onClicked: {
                    backend.set_step(12, checked);
                }
            }

            CheckBox {
                id: step_13

                onClicked: {
                    backend.set_step(13, checked);
                }
            }

            CheckBox {
                id: step_14

                onClicked: {
                    backend.set_step(14, checked);
                }
            }

            CheckBox {
                id: step_15

                onClicked: {
                    backend.set_step(15, checked);
                }
            }
        }
    }
}
