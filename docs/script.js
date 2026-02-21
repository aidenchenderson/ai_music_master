const toggleSwitch=document.getElementById("darkModeSwitch");
const defaultTheme="light";
function themeChange(theme){
  document.documentElement.setAttribute("data-theme", theme);
}
function getStartMode(){
  const saved=localStorage.getItem("theme");
  if (saved === "dark" || saved === "light") return saved;
  return saved;
}
themeChange(getStartMode());
toggleSwitch.addEventListener("click",() =>{
  const currentTheme=document.documentElement.getAttribute("data-theme") || defaultTheme;
  const nextTheme=(currentTheme==="dark")?"light" : "dark";
  themeChange(nextTheme);
  localStorage.setItem("theme",nextTheme);
}
)
const slideImg=document.getElementById("slideImage");
const prevBtn= document.getElementById("prevBtn");
const nextBtn= document.getElementById("nextBtn");
const images=[

  "images/slide1.png",
  "images/slide2.png",
  "images/slide3.png",
  "images/slide4.png",
  "images/slide5.png"

];
let index=0;
function slideDisplay(i){
  if (images.length===0) return;
  index=(i+images.length)%(images.length);
  slideImg.src=images[index];
}
slideDisplay(0);

prevBtn.addEventListener("click",() => slideDisplay(index-1));
nextBtn.addEventListener("click",() => slideDisplay(index+1));