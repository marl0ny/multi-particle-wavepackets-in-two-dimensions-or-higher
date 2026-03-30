const ENUM_CODES = {
    STEPS_PER_FRAME: 0,
    DT: 1,
    T: 2,
    C: 3,
    BRIGHTNESS1: 4,
    BRIGHTNESS2: 5,
    BRIGHTNESS3: 6,
    POTENTIAL_BRIGHTNESS: 7,
    HBAR: 8,
    LINE_DIV1: 9,
    SLIDER_SET_WAVE_FUNC_TITLE: 10,
    M1: 11,
    M2: 12,
    POS1: 13,
    POS2: 14,
    MOMENTUM1: 15,
    MOMENTUM2: 16,
    SIGMA1: 17,
    SIGMA2: 18,
    ENTER_WAVE_FUNC: 19,
    LINE_DIV2: 20,
    LOG2_TEX_WIDTH: 21,
    SLICE_COORD: 22,
    SLICE_IND: 23,
    SAMPLE_IND: 24,
};

function createScalarParameterSlider(
    controls, enumCode, sliderLabelName, type, spec) {
    let label = document.createElement("label");
    label.for = spec['id']
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = `${sliderLabelName} = ${spec.value}`
    controls.appendChild(label);
    let slider = document.createElement("input");
    slider.type = "range";
    slider.style ="width: 95%;"
    for (let k of Object.keys(spec))
        slider[k] = spec[k];
    slider.value = spec.value;
    controls.appendChild(document.createElement("br"));
    controls.appendChild(slider);
    controls.appendChild(document.createElement("br"));
    slider.style.touchAction = 'none';
    if (isOnMobile())
        controls.appendChild(document.createElement("br"));
    slider.addEventListener("input", e => {
        let valueF = Number.parseFloat(e.target.value);
        let valueI = Number.parseInt(e.target.value);
        if (type === "float") {
            label.textContent = `${sliderLabelName} = ${valueF}`
            Module.set_float_param(enumCode, valueF);
        } else if (type === "int") {
            label.textContent = `${sliderLabelName} = ${valueI}`
            Module.set_int_param(enumCode, valueI);
        }
    });
};

gCheckboxXorLists = {};

function createCheckbox(controls, enumCode, name, value, xorListName='') {
    let label = document.createElement("label");
    // label.for = spec['id']
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.innerHTML = `${name}`
    let checkbox = document.createElement("input");
    checkbox.type = "checkbox";
    checkbox.id = `checkbox-${enumCode}`;
    if (xorListName !== '') {
        if (!(xorListName in gCheckboxXorLists))
            gCheckboxXorLists[xorListName] = [checkbox.id];
        else
            gCheckboxXorLists[xorListName].push(checkbox.id);
    }
    // slider.style ="width: 95%;"
    // checkbox.value = value;
    checkbox.checked = value;
    // controls.appendChild(document.createElement("br"));
    controls.appendChild(checkbox);
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
    checkbox.addEventListener("input", e => {
        console.log(e.target.checked);
        Module.set_bool_param(enumCode, e.target.checked);
        if (e.target.checked === true && xorListName !== '') {
            for (let id_ of gCheckboxXorLists[xorListName]) {
                if (id_ !== checkbox.id) {
                    let enumCode2 = parseInt(id_.split('-')[1]);
                    Module.set_bool_param(enumCode2, false);
                    document.getElementById(id_).checked = false;
                }
            }
        }
    }
    );
}

let gVecParams = {};

function createVectorParameterSliders(
    controls, enumCode, sliderLabelName, type, spec) {
    let label = document.createElement("label");
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = `${sliderLabelName} = (${spec.value})`
    gVecParams[sliderLabelName] = spec.value;
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
    for (let i = 0; i < spec.value.length; i++) {
        let slider = document.createElement("input");
        slider.type = "range";
        slider.style ="width: 95%;"
        for (let k of Object.keys(spec))
            slider[k] = spec[k][i];
        slider.value = spec.value[i];
        controls.appendChild(slider);
        controls.appendChild(document.createElement("br"));
        slider.style.touchAction = 'none';
        slider.addEventListener("input", e => {
            let valueF = Number.parseFloat(e.target.value);
            let valueI = Number.parseInt(e.target.value);
            if (type === "Vec2" || 
                type === "Vec3" || type === "Vec4") {
                gVecParams[sliderLabelName][i] = valueF;
                label.textContent 
                    = `${sliderLabelName} = (${gVecParams[sliderLabelName]})`
                Module.set_vec_param(
                    enumCode, spec.value.length, i, valueF);
            } else if (type === "IVec2" || 
                        type === "IVec3" || type === "IVec4") {
                gVecParams[sliderLabelName][i] = valueI;
                label.textContent 
                    = `${sliderLabelName} = (${gVecParams[sliderLabelName]})`
                Module.set_ivec_param(
                    enumCode, spec.value.length, i, valueI);
            }
        });
    }
    if (isOnMobile())
        controls.appendChild(document.createElement("br"));
};

function createSelectionList(
    controls, enumCode, defaultVal, selectionBoxName, textOptions
) {
    let label = document.createElement("label");
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = selectionBoxName;
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
    let selector = document.createElement("select");
    for (let i = 0; i < textOptions.length; i++) {
        let option = document.createElement("option");
        option.value = i;
        option.textContent = textOptions[i];
        selector.add(option);
    }
    selector.value = defaultVal;
    selector.addEventListener("change", e =>
        Module.selection_set(
            enumCode, Number.parseInt(e.target.value))
    );
    controls.appendChild(selector);
    controls.appendChild(document.createElement("br"));
}

function createUploadImage(
    controls, enumCode, name, w_code, h_code
) {
    let label = document.createElement("label");
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = name;
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
    // im.id = `image-${enumCode}`;
    let uploadImage = document.createElement("input");
    uploadImage.type = "file";
    let im = document.createElement("img");
    im.hidden = true;
    let imCanvas = document.createElement("canvas");
    imCanvas.hidden = true;
    controls.appendChild(uploadImage);
    // controls.appendChild(document.createElement("br"));
    controls.appendChild(im);
    // controls.appendChild(document.createElement("br"));
    controls.appendChild(imCanvas);
    // controls.appendChild(document.createElement("br"));
    uploadImage.addEventListener(
        "change", () => {
            console.log("image uploaded");
            const reader = new FileReader();
            reader.onload = e => {
                im.src = e.target.result;
            }
            let loadImageToPotentialFunc = () => {
                let ctx = imCanvas.getContext("2d");
                let width = Module.get_int_param(ENUM_CODES[w_code]);
                let height = Module.get_int_param(ENUM_CODES[h_code]);
                let imW = im.width;
                let imH = im.height;
                imCanvas.setAttribute("width", width);
                imCanvas.setAttribute("height", height);
                let heightOffset = 0;
                let widthOffset = 0;
                if (imW/imH >= width/height) {
                    let ratio = (imW/imH)/(width/height);
                    widthOffset = parseInt(0.5*width*(1.0 - ratio));
                    ctx.drawImage(im, widthOffset, heightOffset,
                                width*(imW/imH)/(width/height), height);
                } else {
                    let ratio = (imH/imW)/(height/width);
                    heightOffset = parseInt(0.5*height*(1.0 - ratio));
                    ctx.drawImage(im, widthOffset, heightOffset,
                                width, (imH/imW)/(height/width)*height);
                }
                let data = ctx.getImageData(0, 0, width, height).data;
                Module.image_set(
                    enumCode, data, width, height);
            }
            let promiseFunc = () => {
                if (im.width === 0 && im.height === 0) {
                    let p = new Promise(() => setTimeout(promiseFunc, 10));
                    return Promise.resolve(p);
                } else {
                    loadImageToPotentialFunc();
                }
            }
            reader.onloadend = () => {
                let p = new Promise(() => setTimeout(promiseFunc, 10));
                Promise.resolve(p);
            }
            reader.readAsDataURL(uploadImage.files[0]);
        }
    );
}

let gUserParams = {};

function modifyUserSliders(enumCode, variableList) {
    if (!(`${enumCode}` in gUserParams))
        gUserParams[`${enumCode}`] = {}; 
    for (let c of variableList) {
        if (!( c in gUserParams[`${enumCode}`]))
            gUserParams[`${enumCode}`][c] = 1.0;
    }
    let userSliders 
        = document.getElementById(`user-sliders-${enumCode}`);
    userSliders.textContent = ``;
    for (let v of variableList) {
        let label = document.createElement("label");
        label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
        label.textContent = `${v} = ${gUserParams[`${enumCode}`][v]}`;
        userSliders.appendChild(label);
        let slider = document.createElement("input");
        slider.type = "range";
        slider.style = "width: 95%;"
        slider.min = "-5";
        slider.max = "5";
        slider.step = "0.01";
        slider.value = gUserParams[`${enumCode}`][v];
        slider.addEventListener("input", e => {
            let value = Number.parseFloat(e.target.value);
            label.textContent = `${v} = ${value}`;
            gUserParams[`${enumCode}`][v] = value;
            Module.set_user_float_param(enumCode, v, value);
        });
        userSliders.appendChild(document.createElement("br"));
        userSliders.appendChild(slider);
        userSliders.appendChild(document.createElement("br"));
    }
}

function createEntryBoxes(
    controls, enumCode, entryBoxName, count, subLabels
) {
    let label = document.createElement("label");
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = entryBoxName;
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
    let entryBoxes = [];
    for (let i = 0; i < count; i++) {
        let entryBox = document.createElement('input');
        entryBox.type = "text";
        entryBox.value = "";
        entryBox.id = `entry-box-${enumCode}-${i}`;
        entryBox.style = "width: 95%;";
        let label = document.createElement("label");
        label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
        label.textContent = `${subLabels[i]}`;
        if (count >= 2) {
            controls.appendChild(label);
            controls.appendChild(document.createElement("br"));
        }
        controls.appendChild(entryBox);
        controls.appendChild(document.createElement("br"));
        entryBoxes.push(entryBox);
        entryBox.addEventListener("input", e =>
            Module.set_string_param(enumCode, i, `${e.target.value}`)
        );
    }
    let userSlidersDiv = document.createElement("div");
    userSlidersDiv.id = `user-sliders-${enumCode}`
    controls.appendChild(userSlidersDiv);

}

function createButton(
    controls, enumCode, buttonName, style=''
) {
    let button = document.createElement("button");
    button.innerText = buttonName;
    if (style !== '')
        button.style = style;
    controls.appendChild(button);
    controls.appendChild(document.createElement("br"));
    button.addEventListener("click", e => Module.button_pressed(enumCode));
}

function createLabel(
    controls, enumCode, labelName, style=''
) {
    let label = document.createElement("label");
    if (style === '')
        label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    else
        label.style = style;
    label.textContent = `${labelName}`;
    label.id = `label-${enumCode}`;
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
}

function editLabel(enumCode, textContent) {
    let idVal = `label-${enumCode}`;
    let label = document.getElementById(idVal);
    label.textContent = textContent;
}

function createLineDivider(controls) {
    let hr = document.createElement("hr");
    hr.style = "color:white;"
    controls.appendChild(hr);
}

let controls = document.getElementById('controls');
createScalarParameterSlider(controls, 0, "Steps/frame", "int", {'value': 1, 'min': 0, 'max': 20});
createScalarParameterSlider(controls, 1, "Time step", "float", {'value': 0.06, 'min': 0.0, 'max': 0.08, 'step': 0.001});
createScalarParameterSlider(controls, 4, "particle brightness 1", "float", {'value': 0.1, 'min': 0.0, 'max': 5.0, 'step': 0.001});
createScalarParameterSlider(controls, 5, "particle brightness 2", "float", {'value': 0.1, 'min': 0.0, 'max': 5.0, 'step': 0.001});
createScalarParameterSlider(controls, 6, "brightness 3", "float", {'value': 0.1, 'min': 0.0, 'max': 5.0, 'step': 0.001});
createScalarParameterSlider(controls, 7, "V(x, y) brightness", "float", {'value': 1.0, 'min': 0.0, 'max': 1.0, 'step': 0.001});
createLineDivider(controls);
createLabel(controls, 10, "Use sliders to initialize ψ(x1, y1; x2, y2):", "color:white; font-family:Arial, Helvetica, sans-serif; font-weight: bold;");
createScalarParameterSlider(controls, 11, "mass 1", "float", {'value': 1.0, 'min': 1.0, 'max': 100.0, 'step': 0.1});
createScalarParameterSlider(controls, 12, "mass 2", "float", {'value': 1.0, 'min': 1.0, 'max': 100.0, 'step': 0.1});
createVectorParameterSliders(controls, 13, "x1, y1 (expressed as fraction of domain side length)", "Vec2", {'value': [0.25, 0.25], 'min': [0.0, 0.0], 'max': [1.0, 1.0], 'step': [0.01, 0.01]});
createVectorParameterSliders(controls, 14, "x2, y2 (expressed as fraction of domain side length)", "Vec2", {'value': [0.75, 0.25], 'min': [0.0, 0.0], 'max': [1.0, 1.0], 'step': [0.01, 0.01]});
createVectorParameterSliders(controls, 15, "momentum 1 (# of π radians)", "Vec2", {'value': [0.0, 0.0], 'min': [-0.25, -0.25], 'max': [0.25, 0.25], 'step': [0.001, 0.001]});
createVectorParameterSliders(controls, 16, "momentum 2 (# of π radians)", "Vec2", {'value': [0.0, 0.0], 'min': [-0.25, -0.25], 'max': [0.25, 0.25], 'step': [0.001, 0.001]});
createScalarParameterSlider(controls, 17, "size 1 (expressed as fraction of domain side length)", "float", {'value': 0.025, 'min': 0.0, 'max': 0.1, 'step': 0.001});
createScalarParameterSlider(controls, 18, "size 2 (expressed as fraction of domain side length)", "float", {'value': 0.025, 'min': 0.0, 'max': 0.1, 'step': 0.001});
createButton(controls, 19, "Initialize new wave function");
createLineDivider(controls);
createScalarParameterSlider(controls, 21, "log2(domain side length)", "int", {'value': 5, 'min': 5, 'max': 7});
createVectorParameterSliders(controls, 22, "Slice coordinate", "Vec2", {'value': [0.5, 0.5], 'min': [0.0, 0.0], 'max': [1.0, 1.0], 'step': [0.01, 0.01]});

