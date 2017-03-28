import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Detect Objects"
    canApply: true

    optionModel: ObjectModel {
    }
    onApply: {
        manager.execute(function(){
            var uri = manager.currentImage;
            genius.detectObjects(uri);
            manager.refreshImage(uri);
            return true;
        });
    }
}
