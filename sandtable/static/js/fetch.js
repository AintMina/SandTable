document.getElementById("formdata").addEventListener("submit", function(e){
    e.preventDefault();
  
    r = document.getElementById("r").value;
    g = document.getElementById("g").value
    b = document.getElementById("b").value
    w = document.getElementById("w").value
	
const formData = new FormData();
//console.log(name);
formData.append('r', r);
formData.append('g', g);
formData.append('b', b);
formData.append('w', w);
formData.append('{{ csrf_token }}');
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