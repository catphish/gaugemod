# Demo
loop do
  Time.now.strftime("|%Y-%m-%d ||%H:%M:%S   |").chars.each.with_index do |c, i|
    f.write(255.chr)
    f.write(3.chr)
    f.write(i.chr)
    f.write(c)
  end
  f.write(255.chr)
  f.write(1.chr)
  f.write(0.chr)
  f.write(rand(254).chr)
  f.write(255.chr)
  f.write(2.chr)
  f.write(0.chr)
  f.write(rand(254).chr)
  sleep 0.2
end

# Sweep to top
f.write(255.chr)
f.write(1.chr)
f.write(5.chr)
f.write(0.chr)

f.write(255.chr)
f.write(2.chr)
f.write(5.chr)
f.write(0.chr)

sleep 2

# Zero
f.write(255.chr)
f.write(1.chr)
f.write(0.chr)
f.write(0.chr)

f.write(255.chr)
f.write(2.chr)
f.write(0.chr)
f.write(0.chr)
