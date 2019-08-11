import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import Sequencer 1.0

ApplicationWindow {
    visible: true
    width: 480
    height: 200
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
                scale: 0.7
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
    }
}
