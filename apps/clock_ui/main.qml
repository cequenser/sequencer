import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import Sequencer 1.0

ApplicationWindow {
    visible: true
    width: 900
    height: 400
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

                property var items: backend.available_midi_ports().split(";")

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
            id: sequencer_parameter_layout

            SpinBox {
                id: sequencer_track_spinbox
                from: 0
                to: 7
                value: 0

                onValueChanged: {
                    backend.set_current_track(value);

                    step_0.checked = backend.is_step_set(0)
                    step_1.checked = backend.is_step_set(1)
                    step_2.checked = backend.is_step_set(2)
                    step_3.checked = backend.is_step_set(3)
                    step_4.checked = backend.is_step_set(4)
                    step_5.checked = backend.is_step_set(5)
                    step_6.checked = backend.is_step_set(6)
                    step_7.checked = backend.is_step_set(7)
                    step_8.checked = backend.is_step_set(8)
                    step_9.checked = backend.is_step_set(9)
                    step_10.checked = backend.is_step_set(10)
                    step_11.checked = backend.is_step_set(11)
                    step_12.checked = backend.is_step_set(12)
                    step_13.checked = backend.is_step_set(13)
                    step_14.checked = backend.is_step_set(14)
                    step_15.checked = backend.is_step_set(15)
                }
            }

            ComboBox {
                width: 250
                model: backend.notes_to_string().split(";")

                onCurrentIndexChanged: {
                    backend.set_note_of_current_track(currentIndex);
                }
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

        RowLayout {
            id: audio_layout

            Button {
                id: audio_record_start_button
                text: qsTr("Record")

                onClicked: {
                    status_label.text = "Recording"
                    backend.start_recording()
                }
            }

            Button {
                id: audio_record_stop_button
                text: qsTr("Stop")

                onClicked: {
                    status_label.text = ""
                    backend.stop_recording()
                }
            }

            Button {
                id: audio_record_play_button
                text: qsTr("Play")

                onClicked: {
                    backend.playback()
                }
            }

            SpinBox {
                id: audio_spinbox
                from: 0
                to: items.length - 1
                value: 0
                Layout.columnSpan: 3

                property var items: backend.available_audio_devices().split(";")

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
                    backend.set_audio_device(value)
                }
            }
        }

        RowLayout {
            id: eq_layout

            SpinBox {
                id: eq_freq_1
                from: 10
                to: 20000
                value: 100
                editable: true

                onValueChanged: {
                    backend.set_eq_freq_1(value)
                }

                onValueModified: {
                    backend.set_eq_freq_1(value)
                }
            }

            SpinBox {
                id: eq_gain_1
                from: -100
                to: 100
                value: 0
                editable: true

                onValueChanged: {
                    backend.set_eq_gain_1(value)
                }

                onValueModified: {
                    backend.set_eq_gain_1(value)
                }
            }

            SpinBox {
                id: eq_q_factor_1
                from: 0
                to: 10000
                value: 100
                editable: true

                onValueChanged: {
                    backend.set_eq_q_factor_1(value)
                }

                onValueModified: {
                    backend.set_eq_q_factor_1(value)
                }
            }

        }
    }
}
