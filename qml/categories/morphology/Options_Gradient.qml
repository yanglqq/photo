import QtQuick 2.2
import QtQml.Models 2.1
import Polka 1.0
import "../../components"
import "../../themes"
import ".."

Options {
    id: container
    title: "Gradient"
    canApply: true

    optionModel: ObjectModel {
        Slider {
            id: iterations
            width: contentWidth
            label: "Number of Iterations"
            minValue: 1
            maxValue: 10
            value: 1
            bins: 9
        }


    }
    onApply: {
        manager.execute(function() {
            var se = ses;
            var uri = morphology.gradient(manager.currentImage, se.elements, se.m, se.n, se.origin, iterations.value);
            manager.addImage(uri, manager.currentImage);
            return true;
        });
    }
}
