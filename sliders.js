const ENUM_CODES = {
    STEPS_PER_FRAME: 0,
    BRIGHTNESS1: 1,
    BRIGHTNESS2: 2,
    BRIGHTNESS3: 3,
    POTENTIAL_BRIGHTNESS: 4,
    HBAR: 5,
    INTERACTION_STRENGTH: 6,
    APPLY_ABSORBING_BOUNDARIES: 7,
    MOUSE_USAGE: 8,
    LINE_DIV1: 9,
    SLIDER_SET_WAVE_FUNC_TITLE: 10,
    SHOW_INITIAL_WAVE_PACKET_AVERAGES: 11,
    SYMMETRY_SELECTION: 12,
    SYMMETRY: 13,
    M1: 14,
    M2: 15,
    POS1: 16,
    MOMENTUM1: 17,
    SIGMA1: 18,
    POS2: 19,
    MOMENTUM2: 20,
    SIGMA2: 21,
    ENTER_WAVE_FUNC: 22,
    LINE_DIV2: 23,
    MAX_LOG2_TEX_WIDTH: 24,
    LOG2_TEX_WIDTH: 25,
    SLICE_IND: 26,
    SAMPLE_IND: 27,
    DT: 28,
    T: 29,
    C: 30,
    PRESET_POTENTIAL_DROPDOWN: 31,
    USER_TEXT_ENTRY: 32,
    LINE_DIV3: 33,
    SHOW3_D: 34,
    HEIGHT1: 35,
    TRANSPARENCY1: 36,
    TRANSPARENCY2: 37,
    HEIGHT3: 38,
    TRANSPARENCY3: 39,
    POTENTIAL_HEIGHT: 40,
};

function createScalarParameterSlider(
    controls, enumCode, sliderLabelName, type, spec) {
    let label = document.createElement("label");
    label.for = spec['id']
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = `${sliderLabelName} = ${spec.value}`;
    label.id = `slider-label-${enumCode}`;
    controls.appendChild(label);
    let slider = document.createElement("input");
    slider.type = "range";
    slider.style ="width: 95%;"
    for (let k of Object.keys(spec))
        slider[k] = spec[k];
    slider.value = spec.value;
    slider.id = `slider-${enumCode}`;
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

function editScalarParameterSliderDisplay(enumCode, sliderLabelName, value) {
    let slider = document.getElementById(`slider-${enumCode}`);
    let label = document.getElementById(`slider-label-${enumCode}`);
    slider.value = value;
    label.textContent 
       = `${sliderLabelName} = ${value}`;
}

function editVectorParameterSliderDisplay(enumCode, sliderLabelName, index, value) {
    let slider = document.getElementById(`slider-${enumCode}-${index}`);
    let label = document.getElementById(`slider-label-${enumCode}`);
    slider.value = value;
    gVecParams[sliderLabelName][Number.parseInt(index)] = value;
    label.textContent 
        = `${sliderLabelName} = (${gVecParams[sliderLabelName]})`;
}

function createVectorParameterSliders(
    controls, enumCode, sliderLabelName, type, spec) {
    let label = document.createElement("label");
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = `${sliderLabelName} = (${spec.value})`;
    label.id = `slider-label-${enumCode}`;
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
        slider.id = `slider-${enumCode}-${i}`;
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
createScalarParameterSlider(controls, 0, "Steps/frame", "int", {'value': 0, 'min': 0, 'max': 20});
createScalarParameterSlider(controls, 1, "particle 1 prob. density brightness", "float", {'value': 0.1, 'min': 0.0, 'max': 5.0, 'step': 0.001});
createScalarParameterSlider(controls, 2, "particle 2 prob. density brightness", "float", {'value': 0.1, 'min': 0.0, 'max': 5.0, 'step': 0.001});
createScalarParameterSlider(controls, 3, "Wave function slice brightness", "float", {'value': 0.1, 'min': 0.0, 'max': 5.0, 'step': 0.001});
createScalarParameterSlider(controls, 4, "Potential brightness", "float", {'value': 1.0, 'min': 0.0, 'max': 1.0, 'step': 0.001});
createScalarParameterSlider(controls, 6, "Interaction strength", "float", {'value': 1.0, 'min': -10.0, 'max': 10.0, 'step': 0.1});
createCheckbox(controls, 7, "Apply absorbing boundaries", true);
createSelectionList(controls, 8, 0, "Mouse usage", [ "None",  "Visualize ψ(x1,  y1; (x2,  y2)=cursor location)",  "———— ψ((x1,  y1)=cursor location; x2,  y2)"]);
createLineDivider(controls);
createLabel(controls, 10, "New ψ(x1, y1; x2, y2) controls:", "color:white; font-family:Arial, Helvetica, sans-serif; font-weight: bold;");
createCheckbox(controls, 11, "Show x(0) and p(0) expectation values with arrows", true);
createSelectionList(controls, 12, 0, "Particle interchange symmetry: ", [ "None",  "Symmetric",  "Antisymmetric"]);
createScalarParameterSlider(controls, 14, "mass 1", "float", {'value': 1.0, 'min': 1.0, 'max': 100.0, 'step': 0.1});
createScalarParameterSlider(controls, 15, "mass 2", "float", {'value': 1.0, 'min': 1.0, 'max': 100.0, 'step': 0.1});
createVectorParameterSliders(controls, 16, "x1, y1 (expressed as fraction of domain side length)", "Vec2", {'value': [0.25, 0.75], 'min': [0.0, 0.0], 'max': [1.0, 1.0], 'step': [0.01, 0.01]});
createVectorParameterSliders(controls, 17, "momentum 1 (π radians)", "Vec2", {'value': [0.075, -0.075], 'min': [-0.25, -0.25], 'max': [0.25, 0.25], 'step': [0.001, 0.001]});
createScalarParameterSlider(controls, 18, "size 1 (expressed as fraction of domain side length)", "float", {'value': 0.045, 'min': 0.02, 'max': 0.1, 'step': 0.001});
createVectorParameterSliders(controls, 19, "x2, y2", "Vec2", {'value': [0.75, 0.25], 'min': [0.0, 0.0], 'max': [1.0, 1.0], 'step': [0.01, 0.01]});
createVectorParameterSliders(controls, 20, "momentum 2", "Vec2", {'value': [-0.075, 0.075], 'min': [-0.25, -0.25], 'max': [0.25, 0.25], 'step': [0.001, 0.001]});
createScalarParameterSlider(controls, 21, "size 2", "float", {'value': 0.045, 'min': 0.02, 'max': 0.1, 'step': 0.001});
createButton(controls, 22, "Initialize new wave function");
createLineDivider(controls);
createScalarParameterSlider(controls, 25, "log2(domain side length)", "int", {'value': 5, 'min': 5, 'max': 7});
createScalarParameterSlider(controls, 28, "Time step", "float", {'value': 0.06, 'min': 0.0, 'max': 0.08, 'step': 0.001});
createSelectionList(controls, 31, 0, "Preset V(x, y, t)", [ "0",  "amp*((x/width)^2 + (y/height)^2)",  "3.8*(step(-y^2+(height*0.04*s1)^2)+step(y^2-(height*0.06*s2)^2))*step(-x^2+(width*0.04*w)^2)",  "1.0/sqrt(x^2+y^2)+1.0/sqrt((x-0.25*width)^2+(y-0.25*height)^2)",  "0.5*(x*cos(w*t/200)/width + y*sin(w*t/200)/height)",  "0.5*(tanh(75.0*(((x/width)^2+(y/height)^2)^0.5-0.45))+1.0)"]);
createEntryBoxes(controls, 32, "Enter potential V(x, y, t) (value clampled to prevent instability)", 1, []);
createLineDivider(controls);
createCheckbox(controls, 34, "Show 3D surface height views", false);
createScalarParameterSlider(controls, 35, "particle prob. density height", "float", {'value': 1.0, 'min': 0.1, 'max': 10.0, 'step': 0.1});
createScalarParameterSlider(controls, 36, "transparency 1", "float", {'value': 0.75, 'min': 0.05, 'max': 1.0, 'step': 0.01});
createScalarParameterSlider(controls, 37, "transparency 2", "float", {'value': 0.25, 'min': 0.05, 'max': 1.0, 'step': 0.01});
createScalarParameterSlider(controls, 38, "Wave function slice height", "float", {'value': 0.25, 'min': 0.05, 'max': 10.0, 'step': 0.1});
createScalarParameterSlider(controls, 39, "Wave function slice transparency", "float", {'value': 1.0, 'min': 0.05, 'max': 1.0, 'step': 0.01});
createScalarParameterSlider(controls, 40, "Potential height", "float", {'value': 1.0, 'min': 0.0, 'max': 10.0, 'step': 0.1});

