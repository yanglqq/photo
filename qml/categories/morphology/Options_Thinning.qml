import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Thinning"
    canApply: true

    property var elementsArray: [
        "0,0,0,-1,1,-1,1,1,1",
        "-1,0,0,1,1,0,1,1,-1",
        "1,-1,0,1,1,0,1,-1,0",
        "1,1,-1,1,1,0,-1,0,0",
        "1,1,1,-1,1,-1,0,0,0",
        "-1,1,1,0,1,1,0,0,-1",
        "0,-1,1,0,1,1,0,-1,1",
        "0,0,-1,0,1,1,-1,1,1"
    ]

    optionModel: ObjectModel {
        Grid {
           columns: 8
           Repeater {
               id: repeater
               model: elementsArray
               StructureElements {
                   id: se
                   width: 30
                   height: 30
                   Component.onCompleted: {
                       var es = container.elementsArray[index].split(",");
                       for (var i = 0; i < es.length; i++) {
                           es[i] = parseInt(es[i]);
                       }
                       se.setElements(es, 3, 3, 4);
                   }
               }
           }
        }
    }
    onApply: {
        manager.execute(function() {
            var array = [];
            var origins = [];
            for (var i = 0; i < 8; i++) {
                var se = repeater.itemAt(i);
                for (var j = 0; j < 9; j++)
                    array.push(se.elements[j]);
                origins.push(se.origin);
            }
            var uri = morphology.thin(manager.currentImage, array, [3,3,3,3,3,3,3,3], [3,3,3,3,3,3,3,3], origins);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
