
var div = document.getElementById('picker')
var red = document.getElementById('red')
var green = document.getElementById('green')
var blue = document.getElementById('blue')
var white = document.getElementById('white')
var red_value = red.innerHTML
var green_value = green.innerHTML
var blue_value = blue.innerHTML
var white_value = white.innerHTML

var colorPicker = new iro.ColorPicker('#picker', {
    // Set the size of the color picker
    width: div.offsetWidth-40,
    // Set the initial color to pure red
    color: {r: red_value, g: green_value, b: blue_value},
    sliderSize: 100,
    handleRadius: 35,
    margin: 20,
    wheelLightness: false
});


var whitePicker = new iro.ColorPicker('#picker', {
    width: div.offsetWidth-40,
    color: {r: white_value, g: white_value, b: white_value},
    sliderSize: 100,
    handleRadius: 35,
    value: 0,
    layout: [
        { 
            component: iro.ui.Slider,
            options: {
                sliderType: 'value'
            }
        },
    ]
});


colorPicker.on('input:end', function(color) {
    // Get RGB values
    var R = color.rgb['r'];
    var G = color.rgb['g'];
    var B = color.rgb['b'];

    // Write value to div
    red.innerHTML = R;
    green.innerHTML = G;
    blue.innerHTML = B;

    // POST values without refreshing
    const formData = new FormData();
    formData.append('r', R);
    formData.append('g', G);
    formData.append('b', B);
    formData.append('csrfmiddlewaretoken', '{{ csrf_token }}');
    console.log(formData);
    fetch('/color/', {
        method: 'POST',
        body: formData
    })
    .then(response => response.json())
    .then(data => {
        console.log('Success:', data);
    })
    .catch(error => {
        console.error('Error:', error);
    });
});
whitePicker.on('input:end', function(color) {
    // Get white value
    var W = parseInt(color.value * 2.555);

    // Write value to div
    white.innerHTML = W;

    // POST values without refreshing
    const formData = new FormData();
    formData.append('w', W);
    formData.append('csrfmiddlewaretoken', '{{ csrf_token }}');
    console.log(formData);
    fetch('/color/', {
        method: 'POST',
        body: formData
    })
    .then(response => response.json())
    .then(data => {
        console.log('Success:', data);
    })
    .catch(error => {
        console.error('Error:', error);
    });
});