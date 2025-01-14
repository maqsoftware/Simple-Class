<?php
/* Template Name: Blog index page */
/*
 * The template for displaying the blog index page (list of blog posts)
 * @package WordPress
 * @subpackage SocialChef
 * @since SocialChef 1.0
 */
get_header('buddypress'); 
SocialChef_Theme_Utils::breadcrumbs();
get_sidebar('under-header');

global $post;
$page_id = $post->ID;
$page_custom_fields = get_post_custom( $page_id);

$page_sidebar_positioning = null;
if (isset($page_custom_fields['page_sidebar_positioning'])) {
	$page_sidebar_positioning = $page_custom_fields['page_sidebar_positioning'][0];
	$page_sidebar_positioning = empty($page_sidebar_positioning) ? '' : $page_sidebar_positioning;
}

$section_class = 'full-width';
if ($page_sidebar_positioning == 'both')
	$section_class = 'one-half';
else if ($page_sidebar_positioning == 'left' || $page_sidebar_positioning == 'right') 
	$section_class = 'three-fourth';

$posts_per_page = get_option('posts_per_page');

if ( get_query_var('paged') ) {
    $paged = get_query_var('paged');
} else if ( get_query_var('page') ) {
    $paged = get_query_var('page');
} else {
    $paged = 1;
}

$page = (get_query_var('paged')) ? get_query_var('paged') : 1;
$args = array(
	'paged'			   => $page,
	'posts_per_page'   => $posts_per_page,
	'offset'           => 0,
	'category'         => '',
	'post_type'        => 'post',
	'post_status'      => 'publish'); 

?>
	<div class="row">
		<?php  if ( have_posts() ) { ?>
		<header class="s-title">
			<h1><?php the_title(); ?></h1>
		</header>
		<?php
		if ($page_sidebar_positioning == 'both' || $page_sidebar_positioning == 'left')
			get_sidebar('left');
		?>
		<section class="content <?php echo esc_attr($section_class); ?>">
			<?php
			query_posts($args);
			if ( have_posts() ) {
			while (have_posts()) : the_post(); ?>			
			<!--blog post-->
			<article id="post-<?php the_ID(); ?>" <?php post_class( ); ?>>
				<div class="entry-meta">
					<div class="date">
						<span class="day"><?php echo get_the_date('d') ?></span> 
						<span class="my"><?php echo get_the_date('M') ?>, <?php echo get_the_date('Y') ?></span>
					</div>
					<div class="avatar">
						<a href="<?php echo esc_url( get_author_posts_url(get_the_author_meta( 'ID' )) ) ?>"><?php echo get_avatar( get_the_author_meta( 'ID' ), 32 ); ?><span><?php echo get_the_author() ?></span></a>
					</div>
				</div>
				<div class="container">
					<?php if ( has_post_thumbnail() ) { ?>
					<div class="entry-featured"><a href="<?php the_permalink() ?>"><?php the_post_thumbnail('featured', array('title' => get_the_title())); ?></a></div>
					<?php } ?>
					<div class="entry-content">
						<h2><a href="<?php the_permalink() ?>" title="<?php the_title(); ?>"><?php the_title(); ?></a></h2>
						<?php the_excerpt(); ?>
					</div>
					<div class="actions">
						<div>
							<div class="category"><i class="ico i-category"></i><?php the_category(' ') ?></div>
							<div class="comments"><i class="ico  i-comments"></i><a href="<?php comments_link(); ?>"><?php comments_number('0','1','%'); ?></a></div>
							<div class="leave_comment"><a href="<?php comments_link(); ?>"><?php _e('Leave a comment', 'socialchef'); ?></a></div>
							<div class="more"><a href="<?php the_permalink() ?>"><?php _e('Read more', 'socialchef'); ?></a></div>
						</div>
					</div>
				</div>
			</article>
			<!--//blog post-->
			<?php endwhile; 
			}
			?>
			<div class="pager">
			<?php 
				global $wp_query;
				SocialChef_Theme_Utils::display_pager($wp_query->max_num_pages); 
				wp_reset_query();
				wp_reset_postdata();				
			?>
			</div>
		</section>
		<?php
		if ($page_sidebar_positioning == 'both' || $page_sidebar_positioning == 'right')
			get_sidebar('right');
		} ?>
	</div><!--//row-->
<?php 	
get_footer( 'buddypress' );